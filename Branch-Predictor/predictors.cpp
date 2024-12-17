#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>

using namespace std;

string inputFilePath = "";
string outputFilePath = "";
unsigned long long correctPredictions = 0; 
unsigned long long numBranches = 0;       

unsigned long long addr;
string behavior, line;
unsigned long long target;

//[5%] Always Taken.
void alwaysTaken(ofstream& outputFile){
  correctPredictions = 0;
  numBranches = 0;       
  ifstream infile(inputFilePath);
  while(getline(infile, line)) {
    numBranches++; 
    stringstream s(line);
    s >> hex >> addr >> behavior >> hex >> target;
    if(behavior == "T"){
      correctPredictions++;  
    }
  }
  outputFile << "" << correctPredictions << "," << numBranches << ";" << endl;
}

//[5%] Always Non-Taken
void alwaysNotTaken(ofstream& outputFile){
  correctPredictions = 0;
  numBranches = 0;       
  ifstream infile(inputFilePath);
  while(getline(infile, line)) {
    numBranches++; // y++
    stringstream s(line);
    s >> hex >> addr >> behavior >> hex >> target;
    if(behavior != "T") {
      correctPredictions++;
    }
  }
  outputFile << "" << correctPredictions << "," << numBranches << ";" << endl;
}

/*
[10%] Bimodal Predictor with a single bit of history stored in each predictor
entry. Determine the prediction accuracy of this predictor for the table size of 16,
32, 128, 256, 512, 1024 and 2048 entries. Assume that the initial state of all
prediction counters is “Taken” (T) 
*/
void oneBitBimodal(ofstream& outputFile){
  vector<int> tableSizes = {16, 32, 128, 256, 512, 1024, 2048};
  vector<string> tableNames = {"table1", "table2", "table3", "table4", "table5", "table6", "table7"};
  ifstream infile(inputFilePath);
  correctPredictions = 0;
  numBranches = 0;
  int mask = 0;

  //Loops through each of the tables with different sizes, and sets mask according to that size
  //Mask is used to get x amount of bits from addr
  for (int i = 0; i < tableSizes.size(); ++i) {
    correctPredictions = 0;
    vector<string> table(tableSizes[i], "T");
    if(i == 0){
      mask = 0xF;
    } else if(i == 1){
      mask = 0x1F;
    } else if(i == 2){
      mask = 0x7F;
    } else if(i == 3){
      mask = 0xFF;
    } else if(i == 4){
      mask = 0x1FF;
    } else if( i == 5){
      mask = 0x3FF;
    } else {
      mask = 0x7FF;
    }
    while (getline(infile, line)) {
        if(i == 0) numBranches++; // Increment branch count
        stringstream s(line);
        s >> hex >> addr >> behavior >> hex >> target;
        int bimodalIndex = addr & mask;

        // Check prediction table at bimodal index and update if necessary
        if (table[bimodalIndex] == behavior) {
            correctPredictions++;
        } else {
            table[bimodalIndex] = behavior;
        }
    }
    outputFile << correctPredictions << "," << numBranches << "; ";
    infile.close();
    infile.open(inputFilePath);
  }
  outputFile << endl;
}

/*
[20%] Bimodal Predictor with 2-bit saturating counters stored in each
predictor entry. Repeat the same experiments as in part (3) above. Assume that
the initial state of all prediction counters is “Strongly Taken” (TT)
*/
void twoBitBimodal(ofstream& outputFile){
  // Define table sizes
  vector<int> tableSizes = {16, 32, 128, 256, 512, 1024, 2048};
  vector<string> tableNames = {"table1", "table2", "table3", "table4", "table5", "table6", "table7"};
  ifstream infile(inputFilePath);
  correctPredictions = 0;
  numBranches = 0;
  int mask = 0;

  // Loop through different tables
  for (int i = 0; i < tableSizes.size(); ++i) {
    correctPredictions = 0;
    vector<string> table(tableSizes[i], "TT");
    if(i == 0){
      mask = 0xF;
    } else if(i == 1){
      mask = 0x1F;
    } else if(i == 2){
      mask = 0x7F;
    } else if(i == 3){
      mask = 0xFF;
    } else if(i == 4){
      mask = 0x1FF;
    } else if( i == 5){
      mask = 0x3FF;
    } else {
      mask = 0x7FF;
    }
    while (getline(infile, line)) {
      if(i == 0) numBranches++;
      stringstream s(line);
      s >> hex >> addr >> behavior >> hex >> target;
      int bimodalIndex = addr & mask;

      // Compare prediction table and actual outcome
      //Correct taken prediction
      if(behavior == "T" && (table[bimodalIndex] == "WT"|| table[bimodalIndex] == "TT")){
          correctPredictions++;
          if(table[bimodalIndex] == "WT")
            table[bimodalIndex] = "TT";
      //Coorect Not taken prediction
      }else if(behavior == "NT" && (table[bimodalIndex] == "WNT" || table[bimodalIndex] == "SNT")){
        correctPredictions++;
        if(table[bimodalIndex] == "WNT")
          table[bimodalIndex] = "SNT";
      //Incorrect Non taken prediction, update table by shifting towards NT
      }else if(behavior == "NT"){
        if(table[bimodalIndex] == "WT")
          table[bimodalIndex] = "WNT";
        if(table[bimodalIndex] == "TT")
          table[bimodalIndex] = "WT";
      //Incorrect Taken prediction, update table by shifting towards T
      }else if(behavior == "T"){
        if(table[bimodalIndex] == "WNT")
          table[bimodalIndex] = "WT";
        if(table[bimodalIndex] == "SNT")
          table[bimodalIndex] = "WNT";
      }
    }
    outputFile << correctPredictions << "," << numBranches << "; ";
    infile.close();
    infile.open(inputFilePath);
  }
  outputFile << endl;
}

/*
[20%] Gshare predictor, where the PC is XOR-ed with the global history bits to
generate the index into the predictor table. Fix the table size at 2048 entries and
determine the prediction accuracy as a function of the number of bits in the
global history register. Vary the history length from 3 bits to 11 bits in 1-bit
increments. Assume that the initial state of all prediction counters is “Strongly
Taken” (TT). The global history register should be initialized to contain all zeroes
(where 0=NT and 1=T). The global history register should be maintained such
that the least significant bit of the register represents the result of the most recent
branch, and the most significant bit of the register represents the result of the
least recent branch in the history.
*/
void gShare(ofstream& outputFile){
  int pcBits = 0;
  int ghrBits = 0;
  int gShareIndx = 0;
  correctPredictions = 0; // x correct predictions
  numBranches = 0;        // y number branches read
  ifstream infile(inputFilePath); // open file for reading 

  vector<vector<string>> gShares = {
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT"),
    vector<string>(2048, "TT")
  };

  vector<vector<int>> ghrs = {
    vector<int>(3, 0),
    vector<int>(4, 0),
    vector<int>(5, 0),
    vector<int>(6, 0),
    vector<int>(7, 0),
    vector<int>(8, 0),
    vector<int>(9, 0),
    vector<int>(10, 0),
    vector<int>(11, 0)
  };

  for(int i = 0; i < 9; i++){
    correctPredictions = 0; 
    numBranches = 0;
    while(getline(infile, line)) {
      numBranches++;
      stringstream s(line);
      s >> hex >> addr >> behavior >> hex >> target;
      pcBits = addr & 2047;
      if (i == 0){
        ghrBits = (ghrs[0][0] << 2) | (ghrs[0][1] << 1) | ghrs[0][2];
      } else if(i == 1){
        ghrBits = (ghrs[1][0] << 3) | (ghrs[1][1] << 2) | (ghrs[1][2] << 1) | ghrs[1][3];
      } else if(i == 2){
        ghrBits = (ghrs[2][0] << 4) | (ghrs[2][1] << 3) | (ghrs[2][2] << 2) | (ghrs[2][3] << 1) | ghrs[2][4];
      } else if(i==3){
        ghrBits = (ghrs[3][0] << 5) | (ghrs[3][1] << 4) | (ghrs[3][2] << 3) | (ghrs[3][3] << 2) | (ghrs[3][4] << 1) | ghrs[3][5];
      } else if(i == 4){
        ghrBits = (ghrs[4][0] << 6) | (ghrs[4][1] << 5) | (ghrs[4][2] << 4) | (ghrs[4][3] << 3) | (ghrs[4][4] << 2) | (ghrs[4][5] << 1) | ghrs[4][6];
      } else if(i == 5){
        ghrBits = (ghrs[5][0] << 7) | (ghrs[5][1] << 6) | (ghrs[5][2] << 5) | (ghrs[5][3] << 4) | (ghrs[5][4] << 3) | (ghrs[5][5] << 2) | (ghrs[5][6] << 1) | ghrs[5][7];  
      } else if(i == 6){
        ghrBits = (ghrs[6][0] << 8) | (ghrs[6][1] << 7) | (ghrs[6][2] << 6) | (ghrs[6][3] << 5) | (ghrs[6][4] << 4) | (ghrs[6][5] << 3) | (ghrs[6][6] << 2) | (ghrs[6][7] << 1) | ghrs[6][8];    
      } else if(i == 7){
        ghrBits = (ghrs[7][0] << 9) | (ghrs[7][1] << 8) | (ghrs[7][2] << 7) | (ghrs[7][3] << 6) | (ghrs[7][4] << 5) | (ghrs[7][5] << 4) | (ghrs[7][6] << 3) | (ghrs[7][7] << 2) | (ghrs[7][8] << 1) | ghrs[7][9];
      } else if(i == 8){
        ghrBits = (ghrs[8][0] << 10) | (ghrs[8][1] << 9) | (ghrs[8][2] << 8) | (ghrs[8][3] << 7) | (ghrs[8][4] << 6) | (ghrs[8][5] << 5) | (ghrs[8][6] << 4) | (ghrs[8][7] << 3) | (ghrs[8][8] << 2) | (ghrs[8][9] << 1) | ghrs[8][10];  
      }
      gShareIndx = (pcBits ^ ghrBits) % 2048;

      //Compare prediction table with actual outcome
      if(behavior == "T" && (gShares[i].at(gShareIndx) == "WT"|| gShares[i].at(gShareIndx) == "TT")){
        correctPredictions++;
        if(gShares[i].at(gShareIndx) == "WT")
          gShares[i].at(gShareIndx) = "TT";
      } else if(behavior == "NT" && (gShares[i].at(gShareIndx) == "WNT" || gShares[i].at(gShareIndx) == "SNT")){
        correctPredictions ++;
        if(gShares[i].at(gShareIndx) == "WNT")
          gShares[i].at(gShareIndx) = "SNT";
      } else if(behavior == "NT" && (gShares[i].at(gShareIndx) == "TT" || gShares[i].at(gShareIndx) == "WT")){
        if(gShares[i].at(gShareIndx) == "WT")
          gShares[i].at(gShareIndx) = "WNT";
        if(gShares[i].at(gShareIndx) == "TT")
          gShares[i].at(gShareIndx) = "WT";  
      }else if(behavior == "T" && (gShares[i].at(gShareIndx) == "SNT" || gShares[i].at(gShareIndx) == "WNT")){
        if(gShares[i].at(gShareIndx) == "WNT")
          gShares[i].at(gShareIndx) = "WT";
        if(gShares[i].at(gShareIndx) == "SNT")
          gShares[i].at(gShareIndx) = "WNT";
      }

      // Update GHR, shifts each entry down 1, then adds new entry at tail end
      for(int j = 1; j<ghrs[i].size(); j++)
          ghrs[i].at(j-1) = ghrs[i].at(j);
      if(behavior == "T")
        ghrs[i].at(ghrs[i].size()-1) = 1;
      else
        ghrs[i].at(ghrs[i].size()-1) = 0;
    }
    outputFile << correctPredictions << "," << numBranches << "; "; 
    infile.close();
    infile.open(inputFilePath);  
  }
  outputFile << endl;
}

/*
[30%] Tournament Predictor. The tournament predictor selects between Gshare and bimodal predictor for every branch. 
Configure Gshare with 2048-entry table and 11 bits of global history, and configure bimodal predictor with 2048-entry table. 
Furthermore, configure the selector table with 2048 entries and use the same index as you use for bimodal predictor to index 
into the selector table (that is, the PC). For each entry in the selector, the two-bit counter encodes the following 
states: 00 – prefer Gshare, 01 – weakly prefer Gshare, 10 – weakly prefer Bimodal, 11 – prefer bimodal. If the two predictors 
provide the same prediction, then the corresponding selector counter remains the same. If one of the predictors is correct and 
the other one is wrong, then the selector’s counter is decremented or incremented to move towards the predictor that was correct. 
Initialize all the component predictors to “Strongly Taken” and initialize the selector’s counters to “Prefer Gshare”.
*/
void tournament(ofstream& outputFile){
  correctPredictions = 0; 
  numBranches = 0;        
  ifstream infile(inputFilePath);
  vector<string> bimodal(2048,"ST");
  vector<string> gShare(2048,"ST");
  vector<string> selector(2048,"SG");
  vector<int> ghr(11,0);

  int bTaken = 0;
  int gTaken = 0;
  int gNTaken = 0;
  int bNTaken = 0;

  while(getline(infile, line)) {
    numBranches++;
    stringstream s(line);
    s >> hex >> addr >> behavior >> hex >> target;
    //Last 11 bits of the PC/Addr
    int bimodalIndex = addr & 2047;
    int selectorIndex = bimodalIndex;  
    int ghrBits = (ghr[0] << 10) | (ghr[1] << 9) | (ghr[2] << 8) | (ghr[3] << 7) | (ghr[4] << 6) | (ghr[5] << 5) | (ghr[6] << 4) | (ghr[7] << 3) | (ghr[8] << 2) | (ghr[9] << 1) | ghr[10]; 
    //Xor PC with GHR
    int gShareIndex = (bimodalIndex ^ ghrBits) % 2048;
    string bimodalPred = bimodal.at(bimodalIndex);
    string gSharePred = gShare.at(gShareIndex);
    string selectorPred = selector.at(selectorIndex);

    //Bimodal Selected
    if(selector.at(selectorIndex) == "SB" || selector.at(selectorIndex) == "WB"){ 
      if(behavior == "T" && (bimodal.at(bimodalIndex) == "WT"|| bimodal.at(bimodalIndex) == "ST")){
          correctPredictions++;
          //More taken bimodal
          if(bimodal.at(bimodalIndex) == "SNT")
            bimodal.at(bimodalIndex) = "WNT";
          if(bimodal.at(bimodalIndex) == "WNT")
            bimodal.at(bimodalIndex) = "WT";
          if(bimodal.at(bimodalIndex) == "WT")
            bimodal.at(bimodalIndex) = "ST";

          //More Bimodal
          if(selector.at(selectorIndex) == "SG")
            selector.at(selectorIndex) = "WG";
          if(selector.at(selectorIndex) == "WG")
            selector.at(selectorIndex) = "WB";
          if(selector.at(selectorIndex) == "WB")
            selector.at(selectorIndex) = "SB";

          //More taken Gshare
          if(gShare.at(gShareIndex) == "SNT")
            gShare.at(gShareIndex) = "WNT";
          if(gShare.at(gShareIndex) == "WNT")
            gShare.at(gShareIndex) = "WT";
          if(gShare.at(gShareIndex) == "WT")
            gShare.at(gShareIndex) = "ST";
      }else if(behavior == "NT" && (bimodal.at(bimodalIndex) == "WNT" || bimodal.at(bimodalIndex) == "SNT")){
          correctPredictions++;
          if(bimodal.at(bimodalIndex) == "ST")
            bimodal.at(bimodalIndex) = "WT";
          if(bimodal.at(bimodalIndex) == "WT")
            bimodal.at(bimodalIndex) = "WNT";
          if(bimodal.at(bimodalIndex) == "WNT")
            bimodal.at(bimodalIndex) = "SNT";

          if(selector.at(selectorIndex) == "SG")
            selector.at(selectorIndex) = "WG";
          if(selector.at(selectorIndex) == "WG")
            selector.at(selectorIndex) = "WB";
          if(selector.at(selectorIndex) == "WB")
            selector.at(selectorIndex) = "SB";

          if(gShare.at(gShareIndex) == "ST")
            gShare.at(gShareIndex) = "WT";
          if(gShare.at(gShareIndex) == "WT")
            gShare.at(gShareIndex) = "WNT";
          if(gShare.at(gShareIndex) == "WNT")
            gShare.at(gShareIndex) = "SNT";

      }else if(behavior == "NT" && (bimodal.at(bimodalIndex) == "ST" || bimodal.at(bimodalIndex) == "WT")){
        //Bimodal less taken
        if(bimodal.at(bimodalIndex) == "ST")
          bimodal.at(bimodalIndex) = "WT";
        if(bimodal.at(bimodalIndex) == "WT")
          bimodal.at(bimodalIndex) = "WNT";
        if(bimodal.at(bimodalIndex) == "WNT")
          bimodal.at(bimodalIndex) = "SNT";

        //less bimodal
        if(selector.at(selectorIndex) == "SB")
          selector.at(selectorIndex) = "WB";
        if(selector.at(selectorIndex) == "WB")
          selector.at(selectorIndex) = "WG";
        if(selector.at(selectorIndex) == "WG")
          selector.at(selectorIndex) = "SG";

        //Gshare less taken
        if(gShare.at(gShareIndex) == "ST")
          gShare.at(gShareIndex) = "WT";
        if(gShare.at(gShareIndex) == "WT")
          gShare.at(gShareIndex) = "WNT";
        if(gShare.at(gShareIndex) == "WNT")
          gShare.at(gShareIndex) = "SNT";   
      }else if(behavior == "T" && (bimodal.at(bimodalIndex) == "SNT" || bimodal.at(bimodalIndex) == "WNT")){
        //Bimodal more taken
        if(bimodal.at(bimodalIndex) == "SNT")
          bimodal.at(bimodalIndex) = "WNT";
        if(bimodal.at(bimodalIndex) == "WNT")
          bimodal.at(bimodalIndex) = "WT";
        if(bimodal.at(bimodalIndex) == "WT")
          bimodal.at(bimodalIndex) = "ST";

        //less bimodal
        if(selector.at(selectorIndex) == "SB")
          selector.at(selectorIndex) = "WB";
        if(selector.at(selectorIndex) == "WB")
          selector.at(selectorIndex) = "WG";
        if(selector.at(selectorIndex) == "WG")
          selector.at(selectorIndex) = "SG";

        //Gshare more taken
        if(gShare.at(gShareIndex) == "SNT")
          gShare.at(gShareIndex) = "WNT";
        if(gShare.at(gShareIndex) == "WNT")
          gShare.at(gShareIndex) = "WT";
        if(gShare.at(gShareIndex) == "WT")
          gShare.at(gShareIndex) = "ST";   
      } 
    //GShare Selected 
    }else if(selector.at(selectorIndex) == "SG" || selector.at(selectorIndex) == "WG"){
      if(behavior == "T" && (gShare.at(gShareIndex) == "WT"|| gShare.at(gShareIndex) == "ST")){
          correctPredictions++;
          //Bimodal more taken
          if(bimodal.at(bimodalIndex) == "SNT")
            bimodal.at(bimodalIndex) = "WNT";
          if(bimodal.at(bimodalIndex) == "WNT")
            bimodal.at(bimodalIndex) = "WT";
          if(bimodal.at(bimodalIndex) == "WT")
            bimodal.at(bimodalIndex) = "ST";

          //More gshare
          if(selector.at(selectorIndex) == "SB")
            selector.at(selectorIndex) = "WB";
          if(selector.at(selectorIndex) == "WB")
            selector.at(selectorIndex) = "WG";
          if(selector.at(selectorIndex) == "WG")
            selector.at(selectorIndex) = "SG";

          //Gshare more taken
          if(gShare.at(gShareIndex) == "SNT")
            gShare.at(gShareIndex) = "WNT";
          if(gShare.at(gShareIndex) == "WNT")
            gShare.at(gShareIndex) = "WT";
          if(gShare.at(gShareIndex) == "WT")
            gShare.at(gShareIndex) = "ST";   

      }else if(behavior == "NT" && (gShare.at(gShareIndex) == "WNT" || gShare.at(gShareIndex) == "SNT")){
          correctPredictions ++;
          //Bimodal less taken
          if(bimodal.at(bimodalIndex) == "ST")
            bimodal.at(bimodalIndex) = "WT";
          if(bimodal.at(bimodalIndex) == "WT")
            bimodal.at(bimodalIndex) = "WNT";
          if(bimodal.at(bimodalIndex) == "WNT")
            bimodal.at(bimodalIndex) = "SNT";

          //More gshare
          if(selector.at(selectorIndex) == "SB")
            selector.at(selectorIndex) = "WB";
          if(selector.at(selectorIndex) == "WB")
            selector.at(selectorIndex) = "WG";
          if(selector.at(selectorIndex) == "WG")
            selector.at(selectorIndex) = "SG";

          //Gshare less taken
          if(gShare.at(gShareIndex) == "SNT")
            gShare.at(gShareIndex) = "WNT";
          if(gShare.at(gShareIndex) == "WNT")
            gShare.at(gShareIndex) = "WT";
          if(gShare.at(gShareIndex) == "WT")
            gShare.at(gShareIndex) = "ST";   
      }else if(behavior == "NT" && (gShare.at(gShareIndex) == "ST" || gShare.at(gShareIndex) == "WT")){
        //Bimodal less taken
        if(bimodal.at(bimodalIndex) == "ST")
          bimodal.at(bimodalIndex) = "WT";
        if(bimodal.at(bimodalIndex) == "WT")
          bimodal.at(bimodalIndex) = "WNT";
        if(bimodal.at(bimodalIndex) == "WNT")
          bimodal.at(bimodalIndex) = "SNT";
  
        //Selector more bimodal
        if(selector.at(selectorIndex) == "SG")
          selector.at(selectorIndex) = "WG";
        if(selector.at(selectorIndex) == "WG")
          selector.at(selectorIndex) = "WB";
        if(selector.at(selectorIndex) == "WB")
          selector.at(selectorIndex) = "SG";

        //Gshare less taken
        if(gShare.at(gShareIndex) == "ST")
            gShare.at(gShareIndex) = "WT";
        if(gShare.at(gShareIndex) == "WT")
          gShare.at(gShareIndex) = "WNT";
        if(gShare.at(gShareIndex) == "WNT")
          gShare.at(gShareIndex) = "SNT";
      }else if(behavior == "T" && (gShare.at(gShareIndex) == "SNT" || gShare.at(gShareIndex) == "WNT")){
        //Bimodal more taken
        if(bimodal.at(bimodalIndex) == "SNT")
          bimodal.at(bimodalIndex) = "WNT";
        if(bimodal.at(bimodalIndex) == "WNT")
          bimodal.at(bimodalIndex) = "WT";
        if(bimodal.at(bimodalIndex) == "WT")
          bimodal.at(bimodalIndex) = "ST";
  
        //Selector more bimodal
        if(selector.at(selectorIndex) == "SG")
          selector.at(selectorIndex) = "WG";
        if(selector.at(selectorIndex) == "WG")
          selector.at(selectorIndex) = "WB";
        if(selector.at(selectorIndex) == "WB")
          selector.at(selectorIndex) = "SB";

        //Gshare more taken
        if(gShare.at(gShareIndex) == "SNT")
            gShare.at(gShareIndex) = "WNT";
        if(gShare.at(gShareIndex) == "WNT")
          gShare.at(gShareIndex) = "WT";
        if(gShare.at(gShareIndex) == "WT")
          gShare.at(gShareIndex) = "ST";   
      }

      //Update GHR
      int i = 0;
      for( i=1; i<ghr.size(); i++)
          ghr.at(i-1) = ghr.at(i);
      if(behavior == "T")
        ghr.at(ghr.size()-1) = 1;
      else
        ghr.at(ghr.size()-1) = 0;
    }

    // if the predictions were the same, revert the selector back to what it initially was, so it just stays the same
    bTaken = 0;
    gTaken = 0;
    gNTaken = 0;
    bNTaken = 0;
    if((bimodalPred == "ST" || bimodalPred == "WT"))
      bTaken = 1;
    if((bimodalPred == "SNT" || bimodalPred == "WNT"))
      bNTaken = 1;
    if((gSharePred == "ST" || gSharePred == "WT"))
      gTaken = 1;
    if((gSharePred == "SNT" || gSharePred == "WNT"))
      gNTaken = 1;

    // If Bimodal and Gshare were taken, reset selector
    if(bTaken == gTaken)
      selector.at(selectorIndex) = selectorPred;
    // if Bimodal and Gshare were not taken, reset selector
    if(bNTaken == gNTaken)
      selector.at(selectorIndex) = selectorPred;    
  }
  outputFile << correctPredictions << "," << numBranches << "; " << endl; 
  infile.close();
  infile.open(inputFilePath);
}

int main(int argc, char *argv[]) {
  if (argc != 3){
        cerr << "Usage: ./predictor <input_file.txt> <output_file.txt>\n";
        exit(1);
  }

  inputFilePath = argv[1];
  outputFilePath = argv[2];
  ofstream outputFile(outputFilePath);
  alwaysTaken(outputFile);
  alwaysNotTaken(outputFile);
  oneBitBimodal(outputFile);
  twoBitBimodal(outputFile);
  gShare(outputFile);
  tournament(outputFile);

  return 0;
}