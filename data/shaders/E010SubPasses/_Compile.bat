glslangValidator -g -V -o GBuffer.vert.spv -H GBuffer.vert > GBuffer.vert.spv.txt
glslangValidator -g -V -o GBuffer.frag.spv -H GBuffer.frag > GBuffer.frag.spv.txt

glslangValidator -g -V -o Composition.vert.spv -H Composition.vert > Composition.vert.spv.txt
glslangValidator -g -V -o Composition.frag.spv -H Composition.frag > Composition.frag.spv.txt

glslangValidator -g -V -o Transparent.vert.spv -H Transparent.vert > Transparent.vert.spv.txt
glslangValidator -g -V -o Transparent.frag.spv -H Transparent.frag > Transparent.frag.spv.txt

pause