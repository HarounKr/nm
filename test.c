int small_data_1;
int small_data_2 = 2;
// Essayez avec différents types et tailles de données
char small_array[4] = {0, 1, 2, 3};
int large_data[1024] = {0}; // Exemple de plus grande donnée

int main() {
    return small_data_1 + small_data_2 + small_array[0] + large_data[0];
}
