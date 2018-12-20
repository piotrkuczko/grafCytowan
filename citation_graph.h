#ifndef CITATION_GRAPH_H
#define CITATION_GRAPH_H

#include <vector>
#include <memory>
#include <set>
#include <map>


class PublicationNotFound : public std::exception {
    const char * what () const noexcept override {
        return "PublicationNotFound";
    }
};

class PublicationAlreadyCreated : public std::exception {
    const char * what () const noexcept override {
        return "PublicationAlreadyCreated";
    }
};

class TriedToRemoveRoot : public std::exception {
    const char * what () const noexcept override {
        return "TriedToRemoveRoot";
    }
};

template <class Publication>
class CitationGraph {

private:
    class Node
            : public std::enable_shared_from_this<Node>
{
    public:
        Publication publication;
        std::set<std::shared_ptr<Node> > children;
        std::set<std::weak_ptr<Node>, std::owner_less<std::weak_ptr<Node> > > parents;
        Node (typename Publication::id_type const &stem_id)
                : publication(stem_id) {}


        ~Node() {
            auto removed = this -> weak_from_this();
            for (auto child : children) {
                child -> parents.erase(removed);
            }

        }
    };

    std::shared_ptr<Node> root;
    std::map <typename Publication::id_type, std::weak_ptr<Node> > map;

public:
    CitationGraph(typename Publication::id_type const &stem_id) {
        root = std::make_shared<Node>(stem_id);
        map[stem_id] = root;
    }

    CitationGraph(CitationGraph<Publication> &&other) noexcept {
        *this = std::move(other);
    }
    CitationGraph<Publication>& operator=(CitationGraph<Publication> &&other) {
        root.swap( other.root);
        map.swap(other.map);
        return *this;
    }

    typename Publication::id_type get_root_id() const noexcept(noexcept(std::declval<Publication>().get_id())) {
        return root -> publication.get_id();


    }

    std::vector<typename Publication::id_type> get_children(typename Publication::id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();

        std::vector<typename Publication::id_type> result;
        std::shared_ptr<Node> myNode = map.at(id).lock();
        for (auto child : myNode -> children) {
            result.push_back(child -> publication.get_id());
        }
        return result;
    }

    std::vector<typename Publication::id_type> get_parents(typename Publication::id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();
        std::vector<typename Publication::id_type> result;
        std::shared_ptr<Node> myNode = map.at(id).lock();
        for (auto parent : myNode -> parents) {
            if (std::shared_ptr<Node> parent2 = parent.lock())
                result.push_back(parent2 -> publication.get_id());
        }
        return result;
    }

    bool exists(typename Publication::id_type const &id) const {
        auto checkNode = map.find(id);
        if (checkNode == map.end()) return false;
        if (checkNode -> second.expired()) return false;
        return true;
    }

    Publication& operator[](typename Publication::id_type const &id) const {
        if (!exists(id))
            throw PublicationNotFound();

        std::shared_ptr<Node> myNode = map.at(id).lock();
        return myNode -> publication;
    }

    void create(typename Publication::id_type const &id, typename Publication::id_type const &parent_id) {
        create(id, std::vector<typename Publication::id_type> {parent_id});
    }
    void create(typename Publication::id_type const &id, std::vector<typename Publication::id_type> const &parent_ids) {
        if (exists(id))
            throw PublicationAlreadyCreated();
        for (auto parent : parent_ids) {
            if (!exists(parent))
                throw PublicationNotFound();
        }

        if(parent_ids.size() == 0) throw PublicationNotFound();

        std::shared_ptr<Node> newNode = std::make_shared<Node>(id);
        map[id] = newNode;
        std::vector<std::shared_ptr<Node> > addedParents;
        try {
            for (auto parent : parent_ids) {
                std::shared_ptr<Node> parentNode = map.at(parent).lock();
                addedParents.push_back(parentNode);
                parentNode -> children.insert(newNode);
                newNode -> parents.insert(parentNode);
            }
        } catch(...) {
            for (auto addedParent : addedParents) {
                addedParent -> children.erase(newNode);
                newNode -> parents.erase(addedParent);
            }
            map.erase(id);
        }
    }

    void add_citation(typename Publication::id_type const &child_id, typename Publication::id_type const &parent_id) {
        if (!exists(child_id) || !exists(parent_id))
            throw PublicationNotFound();
        std::shared_ptr<Node> childNode = map.at(child_id).lock();
        std::shared_ptr<Node> parentNodeLocked = map.at(parent_id).lock();
        auto result1 = childNode -> parents.insert(parentNodeLocked);
        try {
            parentNodeLocked -> children.insert(childNode);
        } catch(...) {
            if (result1.second)
                childNode -> parents.erase(parentNodeLocked);
            throw;
        }


    }

    void remove(typename Publication::id_type const &id) {
        if (!exists(id))
            throw PublicationNotFound();
        if (id == root -> publication.get_id())
            throw TriedToRemoveRoot();
        std::shared_ptr<Node> myNode = map.at(id).lock();
        for (auto parent : myNode -> parents) {
            if (std::shared_ptr<Node> validParent = parent.lock()) {
                validParent -> children.erase(myNode);
            }

        }
    }

};

#endif //CITATION_GRAPH_H
