#include <stdio.h>
#include <string.h>

struct Student {
  char name[50];
  char id[10];
  int age;
  int grades[5];
};

typedef struct {
  char name[50];
  char id[10];
  int age;
  int grades[5];
} student2; // student2 is now an alias of that struct

typedef struct {
  int x;
  int y;
} Point;

void print_student_complex(struct Student current);
void print_student2(student2 student);
void print_points(Point points[10]);

int main() {
  struct Student kevin;
  kevin.age = 40;
  strncpy(kevin.name, "kevin", 5); // direct string assignment is not allowed
  strncpy(kevin.id, "000123123", 10);
  kevin.grades[0] = 4;

  printf("name=%s age=%d\n", kevin.name, kevin.age);
  print_student_complex(kevin);
  student2 lmao;

  strcpy(lmao.name, "nishan");
  print_student2(lmao);

  // Different ways of initializing
  Point p1 = {5, 10};
  Point p2 = {.x = 6, .y = 12};

  printf("x = %d y = %d \n", p1.x, p1.x);

  // can do
  // memory copy
  p1 = p2;

  Point points[10];
  for (int i = 0; i < 10; i++) {
    points[i].x = i;
    points[i].y = 10 - i;
  }

  print_points(points);
}

void print_student_complex(struct Student current) {
  printf("name=%s\n", current.name);
}

void print_student2(student2 student) {
  printf("Easu student %s %d\n", student.name, student.age);
}

void print_points(Point points[]) {
  for (int i = 0; i < 10; i++) {
    printf("p%d, (%d,%d)\n", i, points[i].x, points[i].y);
  }
}
