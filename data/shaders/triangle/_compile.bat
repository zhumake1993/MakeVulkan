glslangValidator -g -V -o Color.vert.spv -H Color.vert > Color.vert.spv.txt
glslangValidator -g -V -o Color.frag.spv -H Color.frag > Color.frag.spv.txt

glslangValidator -g -V -o Tex.vert.spv -H Tex.vert > Tex.vert.spv.txt
glslangValidator -g -V -o Tex.frag.spv -H Tex.frag > Tex.frag.spv.txt

glslangValidator -g -V -o Lit.vert.spv -H Lit.vert > Lit.vert.spv.txt
glslangValidator -g -V -o Lit.frag.spv -H Lit.frag > Lit.frag.spv.txt

pause