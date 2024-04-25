#include <stdio.h>
#include <dlfcn.h>

int main() {
    void *handle = dlopen("libm.so", RTLD_LAZY);
    double (*cosine)(double) = (double (*)(double)) dlsym(handle, "cos");
    printf("The cosine of 2.0 is %f\n", (*cosine)(2.0));
    dlclose(handle);
    return 0;
}
