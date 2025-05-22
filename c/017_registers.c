/*******************************************************************************
*
* Program: Register Demonstration
* 
* Description: Example of using the register storage class in C.
*
* YouTube Lesson: https://www.youtube.com/watch?v=HD1ycaXrivA  
*
* Author: Kevin Browne @ https://portfoliocourses.com
*
*******************************************************************************/
#include <stdio.h>

int main()
{
  // declares a register variable, the variable will be kept in a CPU register 
  // instead of memory, unless the compiler ignores it!

  // From the video it says we should not really do it, because compiler knows better and might optimize betters
  // and infact it may actually slow down, do this if we are writing for a rare new CPU
  register int x = 5;
  
  // We can't get the memory address of a register variable with &x because 
  // the variable is not in memory!  This is a big difference between register
  // variables and other variables.
  // int *y = &x;
  
  // we can print out register variables and perform calculations with them 
  // the same as regular variables
  printf("x: %d\n", x);
  
  return 0;
}