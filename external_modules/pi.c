int run(int num) {
    // Calculates 'num' digits of Pi
    double pi = 0.0;
    int sign = 1;

    for (int i = 0; i < num; i++) {
        pi += (4.0 / (2 * i + 1)) * sign;
        sign = -sign;
    }

    return num;
}