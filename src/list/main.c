/* Example program for stack ADT */
/* Written by Wyatt Carss, January 2011 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

#include "list.h"
#include "useful.h"

int print_identities(List l);
void add_to_list(List l, int num, int selector);
int test(const char *test_description, int number_of_elements, int index, int add_selector, int expect);

int main()
{
  int exit_status = 0; /* if any tests fail, this will be non-zero */
  int result = 0; /* number of errors in a particular test set */
  int status = unset; /* status of program */
  int input_status = unset; /* used to vet input */
  int errors = 0; /* count of successive invalid inputs */
  int selector = 0; /* index into adder function pointer array */
  int choice = 0; /* user's selection */  
  char str[small_buf_size];

  printf("List testing suite go\n");

  while(status != exiting)
  {
    printf("\nChoose an option.\n");
    printf("1. Add to front.\n");
    printf("2. Add to back.\n");
    printf("3. Add before.\n");
    printf("4. Add after.\n");
    printf("5. Randomize.\n");
    printf("6. Exit.\n");

    while(input_status != success)
    {
      printf("\nEnter your choice: ");
      fgets(str, small_buf_size, stdin);
      if(1 == sscanf(str, "%d", &choice))
      {
        input_status = success;
      }
      else
      {
        printf("Invalid selection. Please input a single number.\n");
        errors++;
      }
    }
    input_status = unset;

    switch(choice)
    {
      case 1:
        printf("Using add_to_front.\n");
        selector = 0;
        break;
      case 2:
        printf("Using add_to_back.\n");
        selector = 1;
        break;
      case 3:
        printf("Using add_before.\n");
        selector = 2;
        break;
      case 4:
        printf("Using add_after.\n");
        selector = 3;
        break;
      case 5:
        printf("Using random add functions.\n");
        selector = -1;
        srand(time(NULL));
        break;
      case 6:
        status = exiting;
        continue;
        break;
      default:
        printf("Invalid selection. Please input one of the numbers shown.\n");
        errors++;
        continue;
    }
    errors = 0;

    result += test("remove on empty list", 0, 0, selector, got_null);
    result += test("remove on list with 1 element", 1, 0, selector, got_null);
    result += test("remove from root with 2 elements", 2, 0, selector, success);
    result += test("remove from tail with 2 elements", 2, -1, selector, success);
    result += test("remove from root with 3 elements", 3, 0, selector, success);
    result += test("remove from middle with 3 elements", 3, 1, selector, success);
    result += test("remove from tail with 3 elements", 3, -1, selector, success);    

    result += test("test tail on a high number of elements", 9999, -1, selector, success);
    result += test("test root on a high number of elements", 9999, -1, selector, success);
    result += test("test arbitrary access on a high number of elements", 9999, 4513, selector, success);

    printf("\nTest complete, status: %d.\n", result);
    if(exit_status == 0 && result != 0) exit_status = result;
  }
  
  printf("Thank you for running tests!\n");
  return exit_status;
}

int test(const char *test_description, int number_of_elements, int index, int add_selector, int expect)
{
  int i = 0;
  int status = unset;
  static int test_id = 1;

  List l = list_new();
  add_to_list(l, number_of_elements, add_selector);
  printf("Test %d, '%s':\n", test_id, test_description);
  print_identities(l);

  /* Set the position within the list */
  /* index: tail => -1, root => 0, otherwise n => n */
  if(-1 == index)
    list_tail(l);
  else
  {
    list_root(l);
    for(i = 0; i < index; i++)
      list_next(l);
  }
  list_remove(l);

  /* Return info to the user */
  if(expect == print_identities(l))
  {
    status = success;
    printf("Test %d: success\n", test_id);
  }
  else
  {
    status = fail;
    printf("Test %d: fail\n", test_id);
  }

  /* tear down the list and incremement the test count*/
  list_kill(l);
  test_id++;

  return status;
}

void add_to_list(List l, int num, int selector)
{
  int i = 0;
  int random = false;
  int (*add[4])(List, Node) = 
  {
    list_add_front,
    list_add_back, 
    list_add_before,
    list_add_after
  };

  if(-1 == selector)
  {
    random = true;
  }

  for(i = 0; i < num; i++)
  {
    if(true == random)
    {
      selector = rand() % 4;
    }

    add[selector](l, node_new(NULL, NULL, NULL, NULL, NULL));
  }
}

int print_identities(List l)
{
  Node temp;
  int status = unset;
  //  printf("list has %d elements\n", l->size);

  if(NULL == l || NULL == l->current)
  {
    return got_null;
  }

  temp = l->current;
  list_root(l);
/*  while(null_next != status)
  {
    printf("%d", l->current->id);

    if(l->current->next != NULL)
      printf(", ");
    else
      printf(".\n");

    status = list_next(l);
    //l->current = l->current->next;
  }
*/  l->current = temp;

  return success;
}
