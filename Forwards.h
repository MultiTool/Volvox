#ifndef FORWARDS_H_INCLUDED
#define FORWARDS_H_INCLUDED

class Org;

/* ********************************************************************** */
//template <typename GottaBeANode>
class Link;
typedef Link *LinkPtr;
template <typename GottaBeANode>
struct LinkKit{ //http://www.gotw.ca/gotw/079.htm
//  typedef Link<GottaBeANode>* LinkPtr;
//  typedef std::vector<LinkKit<GottaBeANode>::LinkPtr> LinkVec;
};
typedef std::vector<LinkPtr> LinkVec;

/* ********************************************************************** */
template <typename GottaBeALink=Link>
class Node;
typedef Node<>* NodePtr;
//typedef std::vector<NodePtr> NodeVec;

template <typename GottaBeALink=Link>
struct NodeKit{ //http://www.gotw.ca/gotw/079.htm
  typedef Node<GottaBeALink>* NodePtr;
  typedef std::vector<NodeKit<GottaBeALink>::NodePtr> NodeVec;
};

/* ********************************************************************** */
// test stuff
template <typename gbl>
class Noid{
};
template <typename gbl>
struct np{ //http://www.gotw.ca/gotw/079.htm
  typedef Noid<gbl> *ptr;
};

template <template <typename> class Store, typename T>
struct CachedStore2 {
  Store<T> store;
};

template <typename T>
class base{
};

template <typename T>
class derived:public base<T>{
};


#endif // FORWARDS_H_INCLUDED
