import argparse
import subprocess
import os

def run_command(command, files, output_file):
    # Exécute une commande avec une liste de fichiers et écrit sa sortie dans un fichier spécifié
    try:
        full_command = command + files
        with open(output_file, 'w') as file:
            process = subprocess.Popen(full_command, stdout=file, stderr=subprocess.PIPE, text=True)
            _, stderr = process.communicate()
        if stderr:
            print(f"Error while executing {' '.join(command)}: {stderr}")
    except Exception as e:
        print(f"Error with the command {command}: {e}")

def run_diff(file1, file2):
    try:
        result = subprocess.run(['diff', file1, file2], text=True, capture_output=True)
        if result.stdout:
            print(f"Differences found between {file1} and {file2}:\n{result.stdout}")
        else:
            print(f"No differences detected between {file1} and {file2}.")
    except Exception as e:
        print(f"Error running diff on {file1} and {file2}: {e}")

def main(args, opt):
    for directory in args.directories:
        if os.path.exists(directory):
            files = [os.path.join(directory, f) for f in os.listdir(directory) if os.path.isfile(os.path.join(directory, f))]
            if files:
                ft_nm_output_file = os.path.join(directory, "ft_nm_output.txt")
                nm_output_file = os.path.join(directory, "nm_output.txt")
                print(opt)
                run_command([args.ft_nm] + opt, files, ft_nm_output_file)
                run_command([args.nm] + opt, files, nm_output_file)
                
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
    # parser.add_argument('-opt', nargs=argparse.REMAINDER, help='Options to pass to standard nm and ft_nm')
    args, unknown_args = parser.parse_known_args()
    # args = parser.parse_args()
    main(args, unknown_args)