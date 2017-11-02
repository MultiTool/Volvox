#ifndef CLUSTER_HPP_INCLUDED
#define CLUSTER_HPP_INCLUDED

#include <functional>

#include "Node.hpp"

/* ********************************************************************** */
class Cluster;
typedef Cluster *ClusterPtr;
typedef std::vector<ClusterPtr> ClusterVec;
class Cluster {
public:
  NodeVec NodeList;
  NodeVec *NodeListPtr;
  NodeVec IONodes;
  /* ********************************************************************** */
  Cluster() {
    NodeListPtr = &(NodeList);
  }
  /* ********************************************************************** */
  Cluster(int Num_Nodes) : Cluster() {
    NodeListPtr = &(NodeList);
    Fill_With_Nodes(Num_Nodes);
  }
  /* ********************************************************************** */
  ~Cluster() {
    NodePtr ndp;
    for (size_t ncnt=0; ncnt<this->NodeList.size(); ncnt++) {
      ndp = this->NodeList.at(ncnt);
      delete ndp;
    }
  }
  /* ********************************************************************** */
  void Fill_With_Nodes(size_t Num_Nodes) {
    NodePtr ndp;
    this->NodeList.resize(Num_Nodes);
    for (size_t cnt=0; cnt<Num_Nodes; cnt++) {
      ndp = new Node();
      this->NodeList.at(cnt) = ndp;
    }
    //IONodes.push_back()
  }
  /* ********************************************************************** */
  void Connect_Other_Cluster(ClusterPtr upstreamer) {// Connect upstream cluster all-to-all to this one's inputs
    size_t siz0 = this->NodeList.size();
    size_t siz1 = upstreamer->NodeList.size();
    NodePtr dsn, usn;
    size_t cnt0, cnt1;
    for (cnt0=0; cnt0<siz0; cnt0++) {
      dsn = this->NodeList.at(cnt0);
      for (cnt1=0; cnt1<siz1; cnt1++) {
        usn = upstreamer->NodeList.at(cnt1);
        dsn->ConnectIn(usn);
      }
    }
  }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr genome0) {
    NodePtr ndp;
    for (size_t ncnt=0; ncnt<this->NodeList.size(); ncnt++) {
      ndp = this->NodeList.at(ncnt);
      ndp->Attach_Genome(genome0);
    }
  }
  /* ********************************************************************** */
  void Clear_State() {
    NodePtr ndp;
    size_t siz = this->NodeList.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Clear_State();
    }
  }
  /* ********************************************************************** */
  void Push_Fire() {
    NodePtr ndp;
    size_t siz = this->NodeList.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Push_Fire();
    }
    // right here the fire vals reside in the links
  }
  /* ********************************************************************** */
  void Collect_And_Fire() {
    NodePtr ndp;
    // right here the fire vals reside in the links
    size_t siz = this->NodeList.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Collect_And_Fire();
    }
  }
  /* ********************************************************************** */
  void Fire_Gen() {
    Push_Fire();
    // right here the fire vals reside in the links
    Collect_And_Fire();
    double fred=12;
    //this->Aaagh<double>(fred);
    //this->Aaagh<void>(void);
  }
  /* ********************************************************************** */
  template <class AaaghType> void Aaagh(AaaghType fred) {
    NodePtr ndp;
    size_t siz = this->NodeList.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      //ndp->Aaagh<AaaghType>(fred);
    }
  }
  /* ********************************************************************** */
  void Link_Apply(std::function<void(LinkPtr)> LinkFunc) {
    NodePtr ndp;
    size_t siz = this->NodeList.size();
    for (size_t cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt);
      ndp->Link_Apply(LinkFunc);
    }
  }
  /* ********************************************************************** */
  void Print_Me(int ClusterNum) {
    size_t cnt;
    NodePtr ndp;
    printf(" --------------------------------\n");
    printf(" Cluster ClusterNum:%i, this:%p, ", ClusterNum, this);
    size_t siz = this->NodeList.size();
    printf(" num nodes:%li\n", siz);
    for (cnt=0; cnt<siz; cnt++) {
      ndp = this->NodeList.at(cnt); //ndp->Print_Me();
    }
    printf("\n");
  }
  /* ********************************************************************** */
  void Load_Inputs(VectPtr invec) {
    std::vector<double> *ray = &(invec->ray);
    size_t siz = std::min(ray->size(), this->NodeList.size());//IONodes
    for (size_t cnt=0; cnt<siz; cnt++) {
      this->NodeList.at(cnt)->Load_Input(ray->at(cnt));
    }
  }
  /* ********************************************************************** */
  void Load_Inputs(std::vector<double> *invec) {
    size_t siz = std::min(invec->size(), this->NodeList.size());//IONodes
    for (size_t cnt=0; cnt<siz; cnt++) {
      this->NodeList.at(cnt)->Load_Input(invec->at(cnt));
    }
  }
  /* ********************************************************************** */
  void Load_Inputs(double *invec, size_t siz) {
    siz = std::min(siz, this->NodeList.size());//IONodes
    for (size_t cnt=0; cnt<siz; cnt++) {
      this->NodeList.at(cnt)->Load_Input(invec[cnt]);
    }
  }
  /* ********************************************************************** */
  void Get_Outputs(VectPtr outvec) {
    std::vector<double> *ray = &(outvec->ray);
    size_t siz = std::min(ray->size(), this->NodeList.size());//IONodes
    for (size_t cnt=0; cnt<siz; cnt++) {
      ray->at(cnt) = this->NodeList.at(cnt)->Get_Output();
    }
  }
  /* ********************************************************************** */
  void Get_Outputs(std::vector<double> *outvec) {
    size_t siz = std::min(outvec->size(), this->NodeList.size());//IONodes
    for (size_t cnt=0; cnt<siz; cnt++) {
      outvec->at(cnt) = this->NodeList.at(cnt)->Get_Output();
    }
  }
};

#endif // CLUSTER_HPP_INCLUDED
