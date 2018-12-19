#ifndef GRAFCYTOWAN_CITATION_GRAPH_H
#define GRAFCYTOWAN_CITATION_GRAPH_H

#include <vector>
#include <memory>
#include <set>
#include <map>


struct PublicationNotFound : public std::exception {
    const char * what () const noexcept override {
        return "PublicationNotFound";
    }
};

struct PublicationAlreadyCreated : public std::exception {
    const char * what () const noexcept override {
        return "PublicationAlreadyCreated";
    }
};

struct TriedToRemoveRoot : public std::exception {
    const char * what () const noexcept override {
        return "TriedToRemoveRoot";
    }
};



template <class Publication>
class CitationGraph {

private:
    class Node {
    public:
        Publication::id_type id;
        bool isRoot;
        std::set<std::shared_ptr<Node> > children;      // właściwie to czemu tu nie weak_ptr tylko shared_ptr??
        std::set<std::weak_ptr<Node> > parents;         // BBBudro: "można stracić punkty za trzymanie
        Node (Publication::id_type const &stem_id, bool root)   // internalsów publicznie", to chyba do tego sie odnosi
           : id(stem_id), isRoot(root) {}
    };
    std::shared_ptr<Node> root;
    std::map <Publication::id_type, std::weak_ptr<Node>> map;
public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(Publication::id_type const &stem_id) {
        root = std::make_shared<Node>(stem_id, true);
    }

// Konstruktor przenoszący i przenoszący operator przypisania. Powinny być
// noexcept.
    CitationGraph(CitationGraph<Publication> &&other) noexcept {
        *this = std::move(other);
    }
    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other) {
        *this = std::move(other);
    }

// Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
// wtedy, gdy metoda Publication::get_id jest noexcept. Zamiast pytajnika należy
// wpisać stosowne wyrażenie.
        // https://akrzemi1.wordpress.com/2011/06/10/using-noexcept/
    //Publication::id_type get_root_id() const noexcept(is_nothrow_move_constructible<T>::value && is_nothrow_move_assignable<T>::value);
    Publication::id_type get_root_id() const noexcept(?) {



    }
// Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
// nie istnieje.
    std::vector<Publication::id_type> get_children(Publication::id_type const &id) const {
        if (!exists(id))
            throw (PublicationNotFound);

        std::vector<Publication::id_type> result;    // weak_ptr<X> WEAK;
        if (std::shared_ptr myNode = map.at(id).lock()) {  // WEAK.lock() zwraca shared_ptr na obiekt lub false
            for (auto child : myNode -> children) {     // w sumie to jeśli przeszło exists to chyba nie trzeba tego ifa
                result.push_back(child -> id);          // z linijki wyżej
            }
        }
        return result;
    }

// Zwraca listę identyfikatorów publikacji cytowanych przez publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
// nie istnieje.
    std::vector<Publication::id_type> get_parents(Publication::id_type const &id) const {
        if (!exists(id))
            throw (PublicationNotFound);
        std::vector<Publication::id_type> result;
        std::shared_ptr myNode = map.at(id);
        for (auto parent : myNode -> parents) {     // myNode -> parents to zbiór weak_ptr'ów, parent2 to shared_ptr
            if (std::shared_ptr parent2 = parent.lock())  //  jeśli obiekt wskazywany przez parent istnieje
                result.push_back(parent2 -> id);
        }
    }

// Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(Publication::id_type const &id) const;

// Zwraca referencję do obiektu reprezentującego publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana publikacja
// nie istnieje.
    Publication& operator[](Publication::id_type const &id) const;

// Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
// publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
// parent_ids. Zgłasza wyjątek PublicationAlreadyCreated, jeśli publikacja
// o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound, jeśli
// któryś z wyspecyfikowanych poprzedników nie istnieje.
    void create(Publication::id_type const &id, Publication::id_type const &parent_id);
    void create(Publication::id_type const &id, std::vector<Publication::id_type> const &parent_ids);

// Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
// jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(Publication::id_type const &child_id, Publication::id_type const &parent_id);

// Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
// PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza wyjątek
// TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    void remove(Publication::id_type const &id);

};

#endif //GRAFCYTOWAN_CITATION_GRAPH_H
