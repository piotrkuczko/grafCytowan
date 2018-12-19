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
    private:
        Publication publication;
        std::set<std::shared_ptr<Node> > children;
        std::set<std::weak_ptr<Node> > parents;
        Node (typename Publication::id_type const &stem_id)
                : publication(stem_id) {}
    };

    std::shared_ptr<Node> root;
    std::map <typename Publication::id_type, std::weak_ptr<Node>> map;

public:
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(typename Publication::id_type const &stem_id) {
        root = std::make_shared<Node>(stem_id);
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
    typename Publication::id_type get_root_id() const noexcept(noexcept(std::declval<Publication>().get_id())) {
        return root -> publication.get_id();


    }
// Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
// nie istnieje.
    std::vector<typename Publication::id_type> get_children(typename Publication::id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();

        std::vector<typename Publication::id_type> result;
        std::shared_ptr<Node> myNode = map.at(id).lock();
        for (auto child : myNode -> children) {
            result.push_back(child -> id);
        }
        return result;
    }

// Zwraca listę identyfikatorów publikacji cytowanych przez publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
// nie istnieje.
    std::vector<typename Publication::id_type> get_parents(typename Publication::id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();
        std::vector<typename Publication::id_type> result;
        std::shared_ptr<Node> myNode = map.at(id);
        for (auto parent : myNode -> parents) {
            if (std::shared_ptr<Node> parent2 = parent.lock())
                result.push_back(parent2 -> id);
        }
    }

// Sprawdza, czy publikacja o podanym identyfikatorze istnieje.
    bool exists(typename Publication::id_type const &id) const {
        std::weak_ptr<Node> checkNode = map.find(id);
        if (checkNode == map.end()) return false;
        if (checkNode.expired()) return false;
        return true;
    }

// Zwraca referencję do obiektu reprezentującego publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli żądana publikacja
// nie istnieje.
    Publication& operator[](typename Publication::id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();

        std::shared_ptr<Node> myNode = map.at(id).lock();
        return myNode -> publication;
    }

// Tworzy węzeł reprezentujący nową publikację o identyfikatorze id cytującą
// publikacje o podanym identyfikatorze parent_id lub podanych identyfikatorach
// parent_ids. Zgłasza wyjątek PublicationAlreadyCreated, jeśli publikacja
// o identyfikatorze id już istnieje. Zgłasza wyjątek PublicationNotFound, jeśli
// któryś z wyspecyfikowanych poprzedników nie istnieje.
    void create(typename Publication::id_type const &id, typename Publication::id_type const &parent_id) {
        if (exists(id))
            throw PublicationAlreadyCreated();
        if (!exists(parent_id))
            throw PublicationNotFound();
        // TODO: maybe with add_citation ?

    }
    void create(typename Publication::id_type const &id, std::vector<typename Publication::id_type> const &parent_ids) {
        if (exists(id))
            throw PublicationAlreadyCreated();
        for (auto parent : parent_ids) {
            if (!exists(parent))
                throw PublicationNotFound();
        }
        // TODO: maybe with add_citation ?

    }

// Dodaje nową krawędź w grafie cytowań. Zgłasza wyjątek PublicationNotFound,
// jeśli któraś z podanych publikacji nie istnieje.
    void add_citation(typename Publication::id_type const &child_id, typename Publication::id_type const &parent_id) {
        if (!exists(child_id) || !exists(parent_id))
            throw PublicationNotFound();

    }

// Usuwa publikację o podanym identyfikatorze. Zgłasza wyjątek
// PublicationNotFound, jeśli żądana publikacja nie istnieje. Zgłasza wyjątek
// TriedToRemoveRoot przy próbie usunięcia pierwotnej publikacji.
    void remove(typename Publication::id_type const &id) {
        if (!exists(id))
            throw PublicationNotFound();
        if (id == root -> publication.get_id())
            throw TriedToRemoveRoot();
        // TODO: remove this node from every parent, because parents are only ones who got shared_pointers on this node
        std::shared_ptr<Node> myNode = map.at(id).lock();
        for (auto parent : myNode -> parents) {
            if (std::shared_ptr<Node> validParent = parent.lock()) {

            }

        }
    }

};

#endif //GRAFCYTOWAN_CITATION_GRAPH_H
