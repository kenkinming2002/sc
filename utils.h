#ifndef UTILS_H
#define UTILS_H

// https://stackoverflow.com/questions/15832301/understanding-container-of-macro-in-the-linux-kernel
#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

#endif // UTILS_H
