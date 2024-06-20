import os
import subprocess
import sys
import argparse

def run_command(command, files, output_file, flags, use_valgrind=False):
    # Prépare la commande complète avec Valgrind si nécessaire
    full_command = ([command] + flags + files)
    if use_valgrind:
        valgrind_command = ['valgrind', '--leak-check=full', '--show-leak-kinds=all'] + full_command
    else:
        valgrind_command = full_command
    
    try:
        with open(output_file, 'w') as file:
            process = subprocess.Popen(valgrind_command, stdout=file, stderr=subprocess.PIPE, text=True)
            _, stderr = process.communicate()
        if stderr:
            print(stderr)
    except Exception as e:
        print(f"Error with the command {' '.join(full_command)}: {e}")

def run_diff(file1, file2):
    # Compare deux fichiers et affiche les différences
    try:
        result = subprocess.run(['diff', file1, file2], text=True, capture_output=True)
        if result.stdout:
            print(f"Differences found between {file1} and {file2}:\n{result.stdout}")
        else:
            print(f"No differences detected between {file1} and {file2}.")
    except Exception as e:
        print(f"Error running diff on {file1} and {file2}: {e}")

def main(args, opt):
    use_valgrind = args.valgrind
    for directory in args.directories:
        if os.path.exists(directory):
            files = [os.path.join(directory, f) for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
            if files:
                ft_nm_output_file = os.path.join("./", "ft_nm_output.txt")
                nm_output_file = os.path.join("./", "nm_output.txt")
                run_command(args.ft_nm, files, ft_nm_output_file, opt, use_valgrind)
                run_command(args.nm, files, nm_output_file, opt, use_valgrind)
                
                run_diff(ft_nm_output_file, nm_output_file)
            else:
                print(f"No valid files found in: {directory}")
        else:
            print(f"Specified directory does not exist: {directory}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Compare the outputs of custom nm and the standard nm on specified directories.")
    parser.add_argument('directories', nargs='+', help='List of directories containing files to test')
    parser.add_argument('--ft_nm', type=str, required=True, help='Path to the custom nm executable')
    parser.add_argument('--nm', type=str, required=True, help='Path to the standard nm executable')
    parser.add_argument('--valgrind', action='store_true', help='Use Valgrind to check for memory leaks')
    parser.add_argument('flags', nargs=argparse.REMAINDER, help='Options to pass to nm and ft_nm')
    args = parser.parse_args()
    main(args, args.flags)