#ifndef GRAFCYTOWAN_CITATION_GRAPH_H
#define GRAFCYTOWAN_CITATION_GRAPH_H

#include <vector>





struct PublicationNotFound : public std::exception {
    const char * what () const noexcept {
        return "PublicationNotFound";
    }
};

struct PublicationAlreadyCreated : public std::exception {
    const char * what () const noexcept {
        return "PublicationAlreadyCreated";
    }
};

struct TriedToRemoveRoot : public std::exception {
    const char * what () const noexcept {
        return "TriedToRemoveRoot";
    }
};



template <class Publication>
class CitationGraph {
    // Tworzy nowy graf. Tworzy także węzeł publikacji o identyfikatorze stem_id.
    CitationGraph(Publication::id_type const &stem_id);

// Konstruktor przenoszący i przenoszący operator przypisania. Powinny być
// noexcept.
    CitationGraph(CitationGraph<Publication> &&other);
    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other);

// Zwraca identyfikator źródła. Metoda ta powinna być noexcept wtedy i tylko
// wtedy, gdy metoda Publication::get_id jest noexcept. Zamiast pytajnika należy
// wpisać stosowne wyrażenie.
    Publication::id_type get_root_id() const noexcept(?);

// Zwraca listę identyfikatorów publikacji cytujących publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
// nie istnieje.
    std::vector<Publication::id_type> get_children(Publication::id_type const &id) const;

// Zwraca listę identyfikatorów publikacji cytowanych przez publikację o podanym
// identyfikatorze. Zgłasza wyjątek PublicationNotFound, jeśli dana publikacja
// nie istnieje.
    std::vector<Publication::id_type> get_parents(Publication::id_type const &id) const;

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
