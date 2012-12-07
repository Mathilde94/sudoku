#include <iostream>
#include <vector>
#include <fstream>
#include <time.h>

#define FOR(i,a,b) for(int i=a;i<b;i++)
const int N=9;


using namespace std;

struct cell{//this will represent a case in the sudoku grid
  bool set;//if already filled or not in the initial grid
  vector <int> possible_values;
  int current_value;
};



class Grid{

public:
  cell grid [N][N];
  
  Grid(){// initializing an empty grid
    FOR(i,0,N){
      FOR(j,0,N){
        grid[j][i].set=false;
        grid[j][i].current_value=0;
      }
    } 
  }
  

  void fillGrid(char * nameFile){ // take a file as input (argv)
    ifstream flow(nameFile);
    int n(0);
    FOR(i,0,9){
      char row[10];
      flow >> row;
      FOR(j,0,9){
        int value=(int) row[j]-48;
        grid[i][j].current_value=value;
        grid[i][j].set = (value!=0);
      n+= (value!=0) ? 1:0;
      }
    }
    cout << "Grid with "<<n << " set values:"<<endl;
    
  }

  void display(){// display the grid
    FOR(i,0,N){
      FOR(j,0,N){
        cout << grid[i][j].current_value << " ";
    }
      cout <<endl;
    }
    cout << endl;
  }


  bool notIn(int l,int i, int j){
    /* check if the value l exists in either:
       - the line i
       - the column j
       - the 3*3 block containing the case (i,j)
     */

    FOR(k,0,N){
      if (k!=j){
        if (grid[i][k].current_value==l) {return false;}
      }
    }
    
    FOR(k,0,N){
      if (k!=i){
        if (grid[k][j].current_value==l) {return false;}
      }
    }
    
    int infi=(i/3);
    int infj=(j/3);
    infi*=3;
    infj*=3;
    FOR(k,infi,infi+3){
      FOR(m,infj,infj+3){
        if (k!=i && m!=j){
        if (grid[k][m].current_value==l) {return false;} 
        }
      }
    }
    return true;
  }
  


  bool determine_possible_values(){
    /* double function:
      - determine the possible values for values unset
      - check if wrong input (uncoherence: such as two "2" in a same 3*3 block or line)  
    */

    FOR(i,0,N){
      FOR(j,0,N){
        if (grid[i][j].set==false){
          grid[i][j].possible_values.clear();
          FOR(l,1,10){
            if (notIn(l,i,j)){
              grid[i][j].possible_values.push_back(l);
             }
          }
        }

        else{
          //verify if bad input grid: 
          int valueToCheck =grid[i][j].current_value; 
          if (!notIn(valueToCheck,i,j)){
            cout << "Bad input for the case line:"<<i+1 <<" column:"<<j+1<<" with value: "<< valueToCheck <<endl<<endl;
            return false;
          }
        }
      }
    }
    return true;
  }

  bool recursion(int i,int j, int previous_i, int previous_j){

    /* main part of the algorithm:
      - if studying last cell: if one possible value or value already set: end
      - other cells: try all the possible values in possible_values of the cell:
           - if one value possible: pass to the next cell with this value for the current cell
           - if value not possible, try the next value of the vector possible_values
           - if having tested all the values in possible_values: go back to previous cell, and try again another value for it.
    */

    if (i==N-1 && j==N-1){ //last cell!
      int k=0;
      if (grid[i][j].set==false){
        while (k < grid[i][j].possible_values.size()){
          if (notIn(grid[i][j].possible_values[k],i,j)){
            grid[i][j].current_value=grid[i][j].possible_values[k];
            cout << "Algorithm Finished!"<<endl;
            return true;
          }
          k++;
        }
      }
      else {
        cout << "Algorithm Finished!" <<endl;
        return true;}
      return false;
    }
    
    else{
      if (grid[i][j].set==false){
        int k=0;
        while(k<grid[i][j].possible_values.size()){
          bool result=false;
          int value=grid[i][j].possible_values[k];
          
          if (notIn(value,i,j)){
            grid[i][j].current_value=value;
            int new_i(i),new_j(j);
            if (j==N-1){new_j=0;new_i=i+1;}
            else {new_j=j+1;}
            result = recursion(new_i,new_j,i,j);
            if (result) return true;
            if (!result) {
              grid[i][j].current_value=0;
              if (k==grid[i][j].possible_values.size() -1){ 
                return false;
              }
              
            }
          }
          k+=1;
        }
        if (k>=grid[i][j].possible_values.size()){
          grid[i][j].current_value=0;
          return false;
        }
      }
      else{
        int new_i(i),new_j(j);
        if (j==N-1){new_j=0;new_i=i+1;}
        else {new_j=j+1;}
        return recursion(new_i,new_j,i,j);
      }
    } 
  }



  bool verification(){
    // verify if sum for each line, each column and each 3*3 block is 45

    int result=45;
    FOR(i,0,N){
      int sum_rows=0;
      FOR(j,0,N){
        sum_rows+=grid[i][j].current_value;
      }
      if (sum_rows!=result){
        return false;
      }
    }
    FOR(i,0,N){
      int sum_cols=0;
      FOR(j,0,N){
        sum_cols+=grid[j][i].current_value;
      }
      if (sum_cols!=result){
        return false;
      }
    }
    
    
    for(int i=0;i<9;i+=3){
      for (int j=0;j<9;j+=3){
        int k=0;
        int l=0;
        int sum_block=0;
        FOR(k,0,3){
          FOR (l,0,3){
            sum_block+=grid[i+k][j+l].current_value;
          }
        }
        if (sum_block!=result){return false;}
      }
    }
    return true;
  }
  

};


int main(int arg, char ** argv){

  Grid grid = Grid();
  grid.fillGrid(argv[1]);
  grid.display();
  bool goodInput = grid.determine_possible_values();

  if (goodInput){
    clock_t begin=clock();
    bool result=grid.recursion(0,0,0,0);
    clock_t end=clock();
    cout << "Time used: "<<(float)(end-begin)/1000000.0<<"s." <<endl;
    if (!result) {cout << "There is no solution to this grid." << endl;}
    else{
      cout << "Verification of the correctness ..." <<endl;
      if (grid.verification()){
        cout <<"Solution:" <<endl<<endl;
        grid.display();
      }
      else{
        cout <<" The algorithm found a wrong repartition..." <<endl;
      }
    }
  }

  return 0;
}
