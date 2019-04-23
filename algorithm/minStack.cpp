#include <iostream>
#include <stack>

using namespace std;

class MinStack
{
  public:
    MinStack(){}
    ~MinStack(){}

    void push(int value)
    {
       if(stackData.empty()||value<=getMin())
         stackMin.push(value);
       stackData.push(value);
    }

    void pop()
    {
      if(stackData.empty())
        throw runtime_error("Your stack is empty");
      
      if(stackData.top()==getMin())
        stackMin.pop();

      stackData.pop();
    }

    int top()
    {
      if(stackData.empty())
        throw runtime_error("Your stack is empty");
      else
        return stackData.top();
    }
    int getMin()
    {
       return stackMin.top();       
    }
  private:
    stack<int> stackData;
    stack<int> stackMin;
};

int main()
{
  MinStack stack1;
  stack1.push(3);
  cout << stack1.getMin() << endl; 
  stack1.push(4);
  cout << stack1.getMin() << endl; 
  stack1.pop();
  cout << stack1.getMin() << endl;
}


