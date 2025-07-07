import os
import re

def find_h_files(root_dir):
    h_files = []
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.endswith('.h'):
                h_files.append(os.path.join(dirpath, filename))
    return h_files

def extract_keywords(h_files):
    class_names = set()
    func_names = set()

    class_pattern = re.compile(r'^\s*(class|struct)\s+(\w+)')
    method_pattern = re.compile(r'(\w+)\s*\(')
    public_section_pattern = re.compile(r'\bpublic\s*:')
    private_section_pattern = re.compile(r'\bprivate\s*:')
    end_class_pattern = re.compile(r'\};')

    for header in h_files:
        with open(header, 'r', encoding='utf-8', errors='ignore') as file:
            in_public = False
            current_class = None

            for line in file:
                class_match = class_pattern.search(line)
                if class_match:
                    keyword_type, name = class_match.groups()
                    current_class = name
                    class_names.add(name)
                    continue

                if public_section_pattern.search(line):
                    in_public = True
                    continue
                elif private_section_pattern.search(line) or end_class_pattern.search(line):
                    in_public = False
                    continue

                if in_public:
                    method_match = method_pattern.search(line)
                    if method_match:
                        method_name = method_match.group(1)
                        if method_name != current_class:
                            func_names.add(method_name)

    return sorted(class_names), sorted(func_names)

def generate_keywords_txt(classes, funcs, output_path='keywords.txt'):
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write("#######################################\n")
        f.write("# Syntax Coloring Map For Arduino\n")
        f.write("#######################################\n\n")

        f.write("#######################################\n")
        f.write("# Datatypes (KEYWORD1)\n")
        f.write("#######################################\n\n")

        for cls in classes:
            f.write(f"{cls}\tKEYWORD1\n")

        f.write("\n#######################################\n")
        f.write("# Methods and Functions (KEYWORD2)\n")
        f.write("#######################################\n\n")

        for func in funcs:
            f.write(f"{func}\tKEYWORD2\n")

if __name__ == "__main__":
    headers = find_h_files(".")
    classes, functions = extract_keywords(headers)
    generate_keywords_txt(classes, functions)
    print("keywords.txt gerado com sucesso.")
