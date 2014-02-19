#pragma once

#include "BayesNetEngineExports.h"

#include <vector>

class BayesNetImpl;

//The information the BayesNet class stores can be divided into five types,
//with a complex dependency structure (itself a DAG).
//The first (bottom) type is which nodes exist.
//The second type is which arrows exist, this depends on which nodes exist.
//The third type is the number of values each node has, which again depends
//on which nodes exist.
//The fourth type is the conditional probability distributions for each node,
//known here as the node's 'population'.
//This type depends on which arrows exist, and on the number of values each
//node has.
//The fifth type is the observations made (of some nodes), if any.  This type
//depends on the number of values each node has.
//The dependencies are illustrated in this diagram.

// C   O
// | \ |
// A   V
//  \ /
//   N

//Setters for a type fail if the new information contradicts
//information held about a lower type.  Setters for a type typically delete
//information that depends on that type held about that part of the graph.

//Algorithms can only be run on the graph once all levels of information are
//specified (although for nugatory definitions, the empty (ie original)
//specification is correct).

class BayesNet
{
public:
   BAYESNETENGINE_API BayesNet();
   BAYESNETENGINE_API ~BayesNet();
   BAYESNETENGINE_API BayesNet(const BayesNet &other);
   BAYESNETENGINE_API BayesNet &operator=(const BayesNet &other);

/////////////////////////////Setters///////////////////////////////////////////

   BAYESNETENGINE_API int addNode(unsigned int nodeNo);
   //Fails with 1 if this node number already exists.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int removeNode(unsigned int nodeNo);
   //Fails with 1 if node does not exist.
   //Removes observation and population if they exist,
   //and any arrows to or from the node.
   //Also removes population for any child nodes.
   //Returns 0 on success.

   BAYESNETENGINE_API int specifyNoValues(
      unsigned int nodeNo,unsigned int noValues);
   //Fails with 1 if this node number does not exist,
   //Removes any populations and observations that exist for this node,
   //and removes populations from all child nodes.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int unspecifyNoValues(unsigned int nodeNo);
   //Fails with 1 if node does not exist.
   //Fails with 2 if node's noValues are not specified
   //Removes any populations and observations that exist for this node,
   //and removes populations from all child nodes.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int addArrow(
      unsigned int sourceNodeNo, unsigned int destinationNodeNo);
   //Fails with 1 if either of the nodes don't exist.
   //Fails with 2 if this arrow already exists.
   //Fails with 3 if a cycle would be created.
   //Erases destination node population if it exists.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int removeArrow(
      unsigned int sourceNodeNo, unsigned int destinationNodeNo);
   //Fails with 1 if either of the nodes don't exist.
   //Fails with 2 if the arrow doesn't exist.
   //Erases destination node population if it exists.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int populateNode(
      unsigned int nodeNo,
      const std::vector<std::vector <double>> &distribution);
   //Fails with 1 if the node doesn't exist.
   //Fails with 2 if the node's noValues doesn't exist.
   //Fails with 3 if any of the node's parents' noValues don't exist
   //Fails with 4 if distribution doesn't meet the following criteria
   //distribution.size() is n1 x n2 x ... x nm
   //where ni is the number of elements for the ith parent node.
   //The parents here are ordered as in the result of getParentNodes().
   //The indexing into distribution is such that the nm index changes fastest.
   //Each element of 'distribution' should have the same size,
   //which is the number of elements for this node.
   //Each element of each element of 'distribution' should be between 0 and 1.
   //The sum of the elements in each element of 'distribution' should be 1.
   //Creates a new distribution or overwrites old one.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int depopulateNode(
      unsigned int nodeNo);
   //Fails with 1 if node doesn't exist
   //Fails with 2 if node doesn't have a distribution.
   //Erases the node's distribution.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int observeNode(
      unsigned int nodeNo,
      unsigned int value);
   //Fails with 1 if node doesn't exist.
   //Fails with 2 if node doesn't have a noVals.
   //Fails with 3 if value is greater than or equal to the number
   //of elements for this node.
   //Specifies a new observation or overwrites an old one.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int unobserveNode(
      unsigned int nodeNo);
   //Fails with 1 if node doesn't exist.
   //Fails with 2 if node doesn't have a noVals.
   //Fails with 3 if the node has not been observed.
   //Removes an observation of a node.
   //Returns 0 on success.
   
   BAYESNETENGINE_API int calculateMarginals();
   //Fails with 1 if some node in the net does not have a distribution.
   //'marginals' is populated with a vector<double> for each node in the
   //same order as the result of getNodes.
   //The number elements in this vector<double>
   //is the number of elements for the corresponding node, each element
   //is greater than or equal to 0 and their sum is 1.
   //Returns 0 on success.

/////////////////////////////Getters///////////////////////////////////////////

   BAYESNETENGINE_API std::vector<unsigned int> getNodes()const;
   BAYESNETENGINE_API bool noValuesSpecifiedP(unsigned int nodeNo)const;
   BAYESNETENGINE_API unsigned int getNoValues(unsigned int nodeNo)const;
   BAYESNETENGINE_API std::vector<unsigned int> getChildNodes(
      unsigned int nodeNo)const;
   BAYESNETENGINE_API std::vector<unsigned int> getParentNodes(
      unsigned int nodeNo)const;
   BAYESNETENGINE_API bool distributionSpecifiedP(unsigned int nodeNo)const;
   BAYESNETENGINE_API const std::vector<std::vector<double>> *getPopulation(
      unsigned int nodeNo)const;
   BAYESNETENGINE_API bool nodeObservedP(unsigned int nodeNo)const;
   BAYESNETENGINE_API unsigned int getObservedValue(unsigned int nodeNo)const;
   BAYESNETENGINE_API bool marginalsExistP()const;
   BAYESNETENGINE_API const std::vector<double> *getMarginal(
      unsigned int nodeNo)const;
   BAYESNETENGINE_API unsigned int getUnusedNodeNo()const;

   BAYESNETENGINE_API int getNetStatus()const;
   //Returns 1 if some noValues is not defined, otherwise
   //returns 2 if some population is not defined, otherwise
   //returns 3 if the marginals don't exist
   //(but are ready to be calculated) otherwise
   //returns 4.

private:
   
   BayesNetImpl *impl;
};
