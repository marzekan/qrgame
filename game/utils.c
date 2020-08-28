unsigned short rand_num_gen(unsigned seed)
{
    seed = (1103515245 * seed + 12345) % 2147483648;
    return seed;
}