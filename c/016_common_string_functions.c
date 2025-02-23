#include <stdio.h>
#include <string.h>

/*
 * if strmcmp returns 0, means equal
 * < 0 if 1st nont matching has lower ascii value in s1 than in s2
 * > 0 if  ''''''''''''''''''''' highes scii value in s1 than in s2
 *
 */
int main() {
  char s1[] = "Do u know dae way";
  char s2[] = "Do u know dae way";

  if (strcmp(s1, s2) == 0) {
    printf("s1 and s2 are equal\n");
  }

  // strbrk returns pointer to first occurence of a possible char

  char s[] = "AAASDADAD123344";
  char num[] = "0123456789";

  char *match = strpbrk(s, num); // returns pointer where in s we find any of
                                 // the chars present in nums
  printf("%s\n", match); // going to print 12334l if notthing found returns NULL
}
