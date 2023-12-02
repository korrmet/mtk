#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>

const char* help_msg =
"usage: imutil <name> <files>\n"
"  name  - name of the pair of cpp and hpp with compressed binaries\n"
"  files - list of the source files\n"
"image files are text with pseudographics, where space character or dot means "
"0 and any other character means 1.\n"
"lenght of all lines should be equal.\n"
"names of source files should be valid for using it as name of c variable.\n"
"\nP.S.: this is not the best software that I wrote. I just wanna finish it "
"as soon as possible. maybe in future it would be rewritten and became more "
"reliable and user-friendly, but not now.\n";

int main(int argc, char** argv)
{ if (argc < 2) { std::cout << help_msg; return 0; }
  if (std::string(argv[1]) == "--help") { std::cout << help_msg; return 0; }

  std::ofstream source(std::string(argv[1]).append(".cpp"));
  if (!source.is_open())
  { std::printf("[error] can't create %s\n", 
                std::string(argv[1]).append(".cpp").c_str()); }

  std::ofstream header(std::string(argv[1]).append(".hpp"));
  if (!header.is_open())
  { std::printf("[error] can't create %s\n",
                std::string(argv[1]).append(".hpp").c_str()); }

  std::ofstream source_img(std::string(argv[1]).append("_img.cpp"));
  if (!source_img.is_open())
  { std::printf("[error] can't create %s\n",
                std::string(argv[1]).append("_img.cpp").c_str()); }

  std::ofstream header_img(std::string(argv[1]).append("_img.hpp"));
  if (!header_img.is_open())
  { std::printf("[error] can't create %s\n",
                std::string(argv[1]).append("_img.hpp").c_str()); }

  header << "#ifndef " << std::string(argv[1]) << "_hpp\n"
         << "#define " << std::string(argv[1]) << "_hpp\n"
         << "namespace " << std::string(argv[1]) << " {\n";
  
  source << "#include \"" << std::string(argv[1]).append(".hpp") << "\"\n";

  header_img << "#ifndef " << std::string(argv[1]) << "_img_hpp\n"
             << "#define " << std::string(argv[1]) << "_img_hpp\n"
             << "#include \"mtk.hpp\"\n"
             << "namespace " << std::string(argv[1]) << "_img {\n";

  source_img << "#include \"" 
             << std::string(argv[1]).append("_img.hpp") << "\"\n"
             << "#include \"" << std::string(argv[1]) << ".hpp\"\n";

  for (unsigned int i = 2; i < argc; i++)
  { std::printf("converting image %s ... ", argv[i]);
    std::ifstream file(argv[i]);
    if (!file.is_open()) { std::printf("error, can't open the file\n");
                           continue; }

    unsigned int width = 0;
    unsigned int height = 0;
    std::string line;
    std::string bits;
    while (std::getline(file, line))
    { height++; width = line.length(); bits.append(line); }

    while (bits.size() % 8) { bits.append("."); }

    header << "  extern unsigned char " << std::string(argv[i])
           << "_" << width << "x" << height
           << "[" << bits.size() / 8 << "];\n";

    source << "unsigned char " << std::string(argv[1]) << "::"
           << std::string(argv[i]) << "_" << width << "x" << height 
           << "[" << bits.size() / 8 << "] = \n{ ";

    header_img << "  extern mtk::image " << std::string(argv[i]) << ";\n";

    source_img << "mtk::image " << std::string(argv[1]) << "_img::"
               << std::string(argv[i]) << "("
               << std::string(argv[1]) << "::"
               << std::string(argv[i]) << "_" << width << "x" << height << ", "
               << width << ", " << height << ");\n";
    
    unsigned int bits_counter = 0;
    unsigned int bytes_counter = 0;
    unsigned char current_byte = 0;

    for (char c : bits)
    { if (c != ' ' && c != '.') { current_byte |= 1 << bits_counter; }
      bits_counter++;
      if (bits_counter >= 8)
      { char buf[6];
        std::sprintf(buf, "0x%02x", current_byte);
        source << buf;
        bytes_counter++;
        if (bytes_counter < bits.size() / 8) { source << ", "; }
        if (bytes_counter % 12 == 0 &&
            bytes_counter < bits.size() / 8) { source << "\n  "; }
        bits_counter = 0; current_byte = 0; } }

    source << " };\n"; file.close(); std::cout << "done\n"; }

  header     << "}\n#endif\n";
  header_img << "}\n#endif\n";

  source.close(); header.close(); source_img.close(); header_img.close();
  return 0; }
