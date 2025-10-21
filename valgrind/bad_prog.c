#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Lỗi 1: Cần thay đổi malloc(20 * sizeof(int)) thành malloc(20 * sizeof(int*))
// Lỗi 2: Cần thay đổi free(s->buf) thành:
//          for (int i = 0; i < 20; i++)
//              free(s->buf[i]);
//          free(s->buf);
// Lỗi 3: Cần thay đổi s.flag1 thành s->flag1 !=    0 để tránh cảnh báo sử dụng biến chưa khởi tạo.
typedef struct foo {
    int flag1;
    int flag2;
    int size;
    int **buf;
} foo;

void print_buf(struct foo *s)
{
    printf("buf[0][0]: %d\n", s->buf[0][0]); // Lỗi 2
    free(s->buf); // Lỗi 2
}

void setup_foo(struct foo *s)
{
    s->flag2 = 2;
    s->buf = malloc(20 * sizeof(int));  // Lỗi 1
    for (int i = 0; i < 20; i++)
        s->buf[i] = malloc(20 * sizeof(int));
}

int main(void)
{
    struct foo s;

    setup_foo(&s);
    print_buf(&s);

    if (s.flag1 || s.flag2) // Lỗi 3
        printf("hey\n");

    return 0;
}




// memcheck	Phát hiện lỗi bộ nhớ	
// valgrind --tool=memcheck ./a.out
//valgrind --leak-check=full --show-leak-kinds=all ./malloc_error
//valgrind --leak-check=full --track-origins=yes ./bad_prog

// callgrind	Phân tích hiệu năng	
// valgrind --tool=callgrind ./a.out

// cachegrind	Đo hiệu năng cache CPU	
// valgrind --tool=cachegrind ./a.out

// helgrind	Kiểm tra race condition (đa luồng)	
// valgrind --tool=helgrind ./a.out

// drd	Phát hiện race condition nhẹ	
// valgrind --tool=drd ./a.out

// massif	Theo dõi sử dụng heap	
// valgrind --tool=massif ./a.out

