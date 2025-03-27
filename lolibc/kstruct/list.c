#include "kstruct/list.h"
#include "klib.h"

struct test_struct {
    int age;
    const char *name;
    struct list_head list;
};

struct person
{
    int age;
    struct list_head list;
};

struct person persons[10];

void test_kstruct_list()
{
    printf("here is test\n");

    struct person head, *pos, *n;

    init_list_head(&head.list);
    
    // for (int i = 0; i < 5; i++)
    // {
    //     p = &persons[i];
    //     p->age = i * 10;
    //     list_add(&head.list, &p->list);
    // }

    list_for_each_entry(pos, &head.list, list)
    {
        printf("age = %d\n", pos->age);
    }

    list_for_each_entry_safe(pos, n, &head.list, list)
    {
        if (pos->age == 20)
        {
            list_del(&pos->list);
            break;
        };
    }

    list_for_each_entry(pos, &head.list, list)
    {
        printf("age = %d\n", pos->age);
    }

    // test1 = list_pop(&head.list);
    // test2 = list_pop_tail(&head.list);

    list_for_each_entry(pos, &head.list, list)
    {
        printf("age = %d\n", pos->age);
    }
    // printf("age1 = %d\n age2 = %d\n", test1->age, test2->age);
}

// void dump_list(struct list_head * list){
// 	struct list_head * start = list;
// 	while(true){
// 		list = list->next;
// 		printf("%p", )
// 	}
// }
