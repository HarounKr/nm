import os
import subprocess
import sys

def execute_command_on_files(command, directory):
    # Parcourt tous les fichiers dans le répertoire spécifié
    for filename in os.listdir(directory):
        filepath = os.path.join(directory, filename)
        if os.path.isfile(filepath):
            # Exécute la commande sur le fichier
            try:
                result = subprocess.run([command, filepath], capture_output=True, text=True)
                # Affiche la sortie dans la console
                print(result.stdout)
                if result.stderr:
                    print(f"Errors for {filepath}:\n{result.stderr}\n")
            except Exception as e:
                print(f"Failed to execute {command} on {filepath}: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <command> <directory>")
        sys.exit(1)
    
    command = sys.argv[1]
    directory = sys.argv[2]
    if not os.path.isdir(directory):
        print(f"Error: {directory} is not a valid directory.")
        sys.exit(1)
    
    execute_command_on_files(command, directory)
