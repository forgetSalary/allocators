inline size_t next_power_of_2(size_t num){
    num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num++;
    return num;
}

inline int log_2(size_t num){
    int i;
    for (i = 2; num > 0; ++i) {
        num >>= 1;
    }
    return i-3;
}

#define pow_2(p) (1<<p)