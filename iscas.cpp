#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<list> 
#include<limits>
#include<algorithm> 


using namespace std; 

struct myGates              
 {
	string type;                            //Type of the node: can be INPUT, OUTPUT or GATE
    string nodeName;                                       
	int delay;                             // Delay is the fanout of gates. Inputs will always have a delay of 0 and outputs have a delay of 1. 
 };


list<myGates>nodes;                        //List to store my nodes or gate names 

void printNodes()                           //Function to print the List "nodes". 
    {
        for (const auto& node : nodes) {
            cout<<node.nodeName<<" "<<node.delay<<" "<<node.type<<endl;  
        }
    }

class makeGraph
 {
    public:
    map<string,list<string>> adjacencyList;       //Mapping Gate names with a list of all outputs 

    void addConnectedNodes(const string& sourceNode, const string& destNode)
        {
            adjacencyList[sourceNode].push_back(destNode);    //This will make a list of all inputs and outputs 
        }


    void delayCalculation()                          //Function to calculate the delay of type "GATE" i.e. mid nodes
        {
            for(auto &it : adjacencyList)
                {
                    const string &nodeName = it.first;                  //Get the node name 
                    list<string> &neighbor = it.second;                 //Get the list of neighbors 
                    int delay = static_cast<int>(neighbor.size());      //Get the delay as size of the list 
                    for(auto &gate: nodes)
                        {
                            if(gate.nodeName == nodeName && gate.type =="GATE")
                                {
                                    gate.delay = delay;                  //Assign delay of the node as calculated above 
                                    break; 
                                }
                        }
                }
        }


     void print()                                                       //This function to display the map adjacencyList 
        {
            for(const auto& pairs : adjacencyList){
                cout<<pairs.first;
                for(const string& neighbor :pairs.second){
                    cout<<"--> "<<neighbor<<" "; 
                }
                cout<<endl; 
            }
            cout<<endl; 
        }

    int djikstraAlgo(const string& sourceGate, const string& destGate)
        {    
            list<pair<string, int>>unvisitedNodes;                          //Stores all unvisited gates with name and delay 
            
            for (const auto& gate : nodes) {
                if (gate.type == "OUTPUT" || gate.type == "GATE") {
                    unvisitedNodes.push_back(std::make_pair(gate.nodeName, 9999));       //Stores all the gates with a delay of 9999
                    }
                }

            unvisitedNodes.push_back(make_pair(sourceGate,0));                          //Initialise the source node with delay 0; 

            string startNode = sourceGate;                            
            
            while(!unvisitedNodes.empty())
                {
                    auto minIt = min_element(unvisitedNodes.begin(), unvisitedNodes.end(),           //Compairs 2 nodes and finds the node with lesser delay 
                    [](const pair<string, int>& a, const pair<string, int>& b) {                     
                        return a.second < b.second;
                    });
                    
                    auto currentNode = *minIt;                             //Node pointed to by minIt is stored in currentNode(nodeName,delay)
                    unvisitedNodes.erase(minIt);                           //Node is erased from the unvisitedNodes list
                    
                    if (currentNode.first == destGate) {                   //If current node is destination node print distance else if its 9999 then there is no path between sourceGate and destGate 
                        if(currentNode.second==9999)
                            cout<< "No path from "<< sourceGate <<" to "<< destGate <<endl; 
                        else
                            cout << "Shortest distance from " << sourceGate << " to " << destGate << ": " << currentNode.second << endl;
                        return currentNode.second;
                    }
                    
                    for (const string &neighbor : adjacencyList[currentNode.first]) {               //Find current node in adjacency list and iterate through its output nodes 
                        auto nodeIt = find_if(nodes.begin(), nodes.end(), [neighbor](const myGates &gate) {      //nodeIt points to the output node of the current Node in nodes List 
                            return gate.nodeName == neighbor;
                        });                                            
                             
                        if (nodeIt != nodes.end()) {
                            //cout<<"currentNode: "<<currentNode.first<<" delay "<<currentNode.second<<endl; 
                            //cout<<"nodeIt Name: "<<nodeIt->nodeName<<" nodeIt: " <<nodeIt->delay<<endl; 
                            int distanceToNeighbor = currentNode.second + nodeIt->delay;             //Updates the distance of the node 
                            //cout<<"totaldistance: "<<distanceToNeighbor<<endl; 
                            
                            auto unvisitedIt = find_if(unvisitedNodes.begin(), unvisitedNodes.end(),     
                                [neighbor](const pair<string, int> &p) {                                //Find the node in unvisited list 
                                    return p.first == neighbor;
                                });

                            if (unvisitedIt != unvisitedNodes.end() && distanceToNeighbor < unvisitedIt->second) {      //Update the delay of that node in unvisited list 
                                unvisitedIt->second = distanceToNeighbor;
                                    }
                        }
                    }


                   
                }
                
                cout << "No path found from " << sourceGate << " to " << destGate << "." << endl;
                    return -1;
        }
 }myGraph; 


int parseFile(const string& filename)
  {

	ifstream file(filename); 
	if(!file.is_open())
	  {
	  	cout<<"Wrong file name"<<endl; 
        return 0; 
      }

	string line; 

	while(getline(file,line))
	 {
		if(line.find("INPUT(")==0)                                          //Find the string INPUT and save that node as type INPUT
		  {
			string inputName=line.substr(6,line.find(')')-6);           
            myGates inputGate; 
            inputGate.nodeName = inputName; 
            inputGate.delay = 0; 
			inputGate.type = "INPUT";
            nodes.push_back(inputGate); 

		  }
		else if(line.find("OUTPUT(")==0)                                    //Find the string OUTPUT and save that node as type OUTPUT
		  {
		 	string outputName=line.substr(7,line.find(')') - 7); 
            myGates outputGate; 
            outputGate.nodeName = outputName; 
            outputGate.delay = 1; 
			outputGate.type = "OUTPUT";
            nodes.push_back(outputGate); 

		  }
        else if(line.find("=")!=string::npos)
          {
            if(line.find("G")==0)
                {
                    string intermediateNodes=line.substr(0,line.find(" =")- 0);
                    bool ifExist = false; 
                    for(const auto& gate : nodes)
                        {
                            if(gate.nodeName == intermediateNodes && (gate.type=="INPUT" || gate.type=="OUTPUT")){
                                ifExist = true; 
                                break;
                            }
                        }
                    if (!ifExist)                                         //Find the '='  sign and save the node before it as type GATE 
                        {   myGates middleNodes; 
                            middleNodes.nodeName=intermediateNodes;
                            middleNodes.type="GATE";
                            middleNodes.delay=std::numeric_limits<int>::max();
                            nodes.push_back(middleNodes); 
                            
                        } 

                    size_t posOfEq = line.find("=");       
                    string findExp = line.substr(posOfEq+2); 
                    while(findExp.find("G") !=string::npos)
                        {
                            size_t start = findExp.find("G");                     //Find the '=' sign and find the gates inside ()
                            size_t end = findExp.find(")"); 
                            string InputsOfGate = findExp.substr(start, end-1 - start + 1); 
                            InputsOfGate.erase(remove_if(InputsOfGate.begin(), InputsOfGate.end(), ::isspace), InputsOfGate.end());
                            size_t pos = 0; 
                            while(pos<InputsOfGate.length()){
                                size_t commaPos = InputsOfGate.find(',',pos); 
                                if(commaPos == std::string::npos)
                                    commaPos = InputsOfGate.length(); 

                                string gateName = InputsOfGate.substr(pos,commaPos - pos);
                                myGraph.addConnectedNodes(gateName,intermediateNodes);        //Adds connection between the gates   
                                pos = commaPos+1; 
                                }
                        
                            findExp = findExp.substr(end+1); 
                        }

                        

                }
            }     
     
    }
	file.close(); 
    return 1; 
   
}




int main(int argc, char* argv[])
 {
	if(argc !=4)
	  {

		std::cout<<"Incorrect number of argument"<<std::endl; 

	  }
	else
       {
            string fileName = argv[1]; 
            string inputGate = argv[2]; 
            string outputGate = argv[3]; 
            
            int parse_result = parseFile(fileName); 

            if(parse_result)
               {   

                    bool inputFound = false; 
                    bool outputFound = false; 

                    for(const auto& gate : nodes){
                        if(gate.nodeName == inputGate){
                             inputFound = true;
                            if(gate.type != "INPUT")
                               cout<<"Signal "<<inputGate<<" not an input pin"<<endl;  
                          } 
                        if (gate.nodeName == outputGate){
                            outputFound = true;
                            if(gate.type != "OUTPUT")
                                cout<<"Signal "<<outputGate<<" not an output pin"<<endl;  
   
                          }

                        } 

                    if(!inputFound)
                        cout<<"Signal "<<inputGate<<" not found in file "<< fileName<<endl;
                    if (!outputFound)
                        cout<<"Signal "<<outputGate<<" not found in file "<<fileName<<endl; 
                }

            myGraph.delayCalculation(); 
            //printNodes(); 
            //myGraph.print(); 
            myGraph.djikstraAlgo(inputGate, outputGate); 
	
        }  

  }
