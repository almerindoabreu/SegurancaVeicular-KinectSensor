## DISPOSITIVO DE SEGURANÇA VEICULAR COM LEITURA DE EXPRESSÃO FACIAL PARA PREVENÇÃO DE ACIDENTES POR SONO AO VOLANTE

Esse protótipo de segurança veicular foi um trabalho de conclusão de curso que foi engrenado em pesquisa acadêmica pela Faculdade Salesiana Maria Auxiliadora. Tendo como participantes: Almerindo Abreu, Raphael Marques e como Orientador, o DSc Irineu Neto.

[Pesquisa Acadêmica - Faculdade Salesiana Maria Auxiliadora](http://www.fsma.edu.br/site/projetos/prototipacao-de-um-sistema-de-seguranca-veicular-para-alertas-contra-o-sono-e-cansaco-via-reconhecimento-de-imagens/)


### Objetivo do Projeto

O referido projeto visa elaborar um protótipo computacional capaz de gerar alertas contra o sono e cansaço ao volante, baseado em análise de imagens e técnicas de análise de dados. O sono é definido como um estado funcional, com comportamentos corporais característicos que afetam a mobilidade, caracterizado por baixo reflexo, muitas vezes associado ao cansaço e stress. Então, há reações singulares de expressões faciais ditas universais e determinantes para expressar o sono, o cansaço e o stress, que podem ser aplicadas em sistemas para a prevenção de acidentes automobilísticos.

### Regras de Desenvolvimento do Protótipo

Para o referente código, foi utilizado linguagem C++ em projeto criado no Visual Studio 15 no qual pode ser realizado o download no site [Homepage Visual Studio](https://visualstudio.microsoft.com/pt-br/vs/older-downloads/). A aquisição de dados foi realizada pelo dispositivo de movimento do Xbox 360 Microsoft, o Kinect Sensor.

![Dagrama de Blocos do Protótipo com o Kinect Sensor](http://oi68.tinypic.com/282nggk.jpg)

O projeto é composto pelas libs OpenCV 3.2, OpenNI 2 (NITE 2), e DLIB 19.4. Onde podem ser realizados o seus respectivos download nos links abaixo.

|Bibliotecas | Download |
|---------------|--------|
|DLIB | [Link](http://dlib.net/)|
|OpenCV|[Link](https://opencv.org/opencv-3-4.html)|
|OpenNI | [Link](http://openni.ru/openni-sdk/index.html)

Para executar a instalação do OpenCV, deve ser realizada o download do **[CMake](https://cmake.org/download)** (Instalar em C:\CMake) que é um software que realiza a geração automática de configurações referente ao OpenCV, e o **[MinGW-64](https://sourceforge.net/projects/mingw-w64/files/latest/download)** (instalar no endereço padrão C:\Program Files\mingw-w64)

O **MinGW** (**Min**imalist **G**NU for **W**indows) é uma versão portada  "Portabilidade (informática)") para Microsoft Windows  "Microsoft Windows") do [conjunto de ferramentas GNU "Conjunto de ferramentas GNU"). Este software inclui um conjunto de arquivos cabeçalho para a API "API") do Windows que permite aos desenvolvedores usar o GCC para criar programas nativos em Windows sem precisar contar com uma emulação em tempo real de um sistema Unix-like. É como se fosse um compilador.


    

Utiliza-se o CMake para gerenciar a compilação dos tatos files da biblioteca OpenCV e Dlib.

**Utilização do CMaker**

![enter image description here](http://oi63.tinypic.com/5y4tv7.jpg)

1: Arquivo sources do openCV C:/opencv/sources

2: Pasta onde será gerado os arquivos (chamei a pasta de build_opencv) C:/opencv/build_opencv

3: Para configurar clicar no botão Configure e escolher a IDE que será utilizado para criar a programação, nesse caso é utilizado o Visual Studio 14 2015

4: Clickar em Generate para criar

5: Posterior ao processo de gerar os arquivos clickar em Open Project e buildar o arquivo no Visual Studio com a configuração em Release na plataforma Win32.
![enter image description here](http://oi66.tinypic.com/ics0ft.jpg)



Na IDE do Visual Studio, em propriedades do projeto deve ser inserido os endereços das seguintes Libs mencionada abaixo em: **Configuration Properties > Linker > General > Additional Library Directories**

|Exemplo dos endereços  
|---------------|
|C:\opencv\openNI\install\x64\vc14\lib;  |
|C:\Program Files\PrimeSense\NiTE2\Lib; |
|C:\Program Files\OpenNI2\Lib|
|C:\dlib-19.4\dlib-19.4\|

As bibliotecas e seus módulos devem ser inseridas em **Configuration Properties > Linker > Input > Additional Denpendencies**.

|Bibliotecas| 
|---------------|
|OpenNI2.lib;|
|opencv_core320d.lib;|
|opencv_calib3d320d.lib;|
|opencv_features2d320d.lib;|
|opencv_flann320d.lib;|
|opencv_highgui320d.lib;|
|opencv_imgcodecs320d.lib;|
|opencv_imgproc320d.lib;|
|opencv_ml320d.lib;|
|opencv_objdetect320d.lib;|
|opencv_photo320d.lib;|
|opencv_shape320d.lib;|
|opencv_stitching320d.lib;|
|opencv_superres320d.lib;|
|opencv_video320d.lib;|
|opencv_videoio320d.lib;|
|opencv_videostab320d.lib;

Em **Configuration Properties > C/C++ > General > Additional Includes Directories**.

|Includes Adicionais| 
|---------------|
|C:\opencv\openNI\install\include;|
|C:\Program Files\PrimeSense\NiTE2\Include;|
|C:\Program Files\OpenNI2\Include;|
|C:\dlib-19.4\dlib-19.4\|

Caso de duvidas há um procedimento de instalação das libs tanto no site da OpenCV como no DLIB.

https://docs.opencv.org/2.4/doc/tutorials/introduction/windows_visual_studio_Opencv/windows_visual_studio_Opencv.html
http://dlib.net/compile.html
