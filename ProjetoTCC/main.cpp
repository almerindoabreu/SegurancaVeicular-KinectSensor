//Aquisição do Kinect, Detecção Facial e Manipulação DLIB 
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <windows.h>

#include <dlib/opencv.h>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_processing/generic_image.h>
#include <dlib/image_io.h>
#include <OpenNI.h>

using namespace dlib;
using namespace std;
using namespace openni;
using namespace cv;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

using Time = std::chrono::steady_clock;
using ms = std::chrono::milliseconds;

const openni::SensorType tipoDoSensor = openni::SENSOR_IR;

image_window win;
double segundos;
int x = 0;
float valorMaxOlho = 0;
float valorMinOlho = 0;
int flagValorMax = 0;
int flagValorMin = 0;
int indiceDeBaixoPerclos = 0;
float matrizRA[2][199];



float monitoramentoFPS(time_t inicio, int contador, float fps) {
	time_t fim;
	time(&fim);

	if (((contador + 1) % 60) == 0) {
		segundos = difftime(fim, inicio);
		fps = 60 / segundos;
	}

	dlib::rectangle r(2, 2, 2, 2);
	win.add_overlay(r, rgb_pixel(255, 0, 0), "FPS: " + std::to_string(fps));
	return fps;
}

float razaoDeAspecto(full_object_detection shape) {
	float razaoDeAspecto;

	//RAD = RAZÃO DE ASPECTO ESQUERDA
	float RA_direita = length((shape.part(43) + shape.part(44)) / 2 - (shape.part(47) + shape.part(46)) / 2)
		/ length(shape.part(42) - shape.part(45));
	//string s = "IMGD-" + std::to_string(x) + ": " + std::to_string(razaoDeAspecto);
	//cout << s << endl;

	//RAE = RAZÃO DE ASPECTO ESQUERDA
	float RA_esquerda = length((shape.part(37) + shape.part(38)) / 2 - (shape.part(40) + shape.part(41)) / 2)
		/ length(shape.part(36) - shape.part(39));
	//s = "IMGE-" + std::to_string(x) + ": " + std::to_string(razaoDeAspecto);
	//cout << s << endl;
	razaoDeAspecto = ((RA_direita + RA_esquerda) / 2);
	return razaoDeAspecto;
}

float desvioPadrao(int x) {
	float variancia, media;
	variancia = 0;
	media = 0;
	int i, t;
	for (i = 0; i <= x; i++) {
		media = media + matrizRA[0][i];
	}

	media = media / (i + 1);

	for (t = 0; t <= x; t++) {
		variancia = variancia + pow((matrizRA[0][t] + media), 2);
	}
	variancia = variancia / (t + 1);

	float devPadrao = sqrt(variancia);
	return devPadrao;
}

float erroPadrao(int x) {
	float erroPadrao;
	if (x > 0) {
		erroPadrao = matrizRA[1][x] / sqrt(x);
	}
	else {
		erroPadrao = 0;
	}
	return erroPadrao;
}

void calibragemAberturaOlhos(float matrizRA[2][199]) {
	int x = 0;

	for (x = 0; x <= 199; x++) {
			if (valorMaxOlho < matrizRA[0][x]) {
				valorMaxOlho = matrizRA[0][x];
				flagValorMax = x;
			}
			if (valorMinOlho > matrizRA[0][x] || valorMinOlho == 0) {
				valorMinOlho = matrizRA[0][x];
				flagValorMin = x;
			}
		}

		valorMaxOlho = matrizRA[0][flagValorMax] - matrizRA[2][flagValorMax];
		valorMinOlho = matrizRA[0][flagValorMin] + matrizRA[2][flagValorMin];

		string s = "Valor Máximo" + std::to_string(flagValorMax) + ": " + std::to_string(valorMaxOlho) +
			" -- Valor Mínimo" + std::to_string(flagValorMin) + ": " + std::to_string(flagValorMin);
		cout << s << endl;
}

void tirarFoto(Mat imagem, int x, time_t inicio) {
	time_t final;
	time(&final);

	float seg = difftime(final, inicio);
	//limitar os 20 segundos de amostra.
	if (seg <= 20) {
		cv::imwrite("IMG-" + std::to_string(x) + ".png", imagem);
	}
}

float porcentagemAberturaDosOlhos(full_object_detection shape, float aberturaMax, float aberturaMin, int x) {
	float aberturaDosOlhos;
	float porcentagem;

	//Calcular razão de aspecto da área do olho do sujeito.
	aberturaDosOlhos = razaoDeAspecto(shape);

	//Calcular a porcentagem
	porcentagem = (aberturaDosOlhos - aberturaMin) * 100 / (aberturaMax - aberturaMin);

	//Dados de amostragem
	string s = "IMG-" + std::to_string(x - 199) + ": " + std::to_string(porcentagem) +" % - Valor da Razão de Aspecto: " + std::to_string(aberturaDosOlhos);
	cout << s << endl;

	return porcentagem;
}

int janelaDeAvaliacaoPerclos(float porcentagem) {
	if (porcentagem <= 80) {
		indiceDeBaixoPerclos = indiceDeBaixoPerclos + 1;
	}
	else{
		indiceDeBaixoPerclos = 0;
	}
	if (indiceDeBaixoPerclos > 20) {
		Beep(700, 5000);
	}
	return indiceDeBaixoPerclos;
}

/*
void calibragemAberturaOlhos(full_object_detection shape, int x) {
		
	float aberturaDosOlhos;

	aberturaDosOlhos = razaoDeAspecto(shape);
	//aberturaDosOlhos = length(shape.part(44) - shape.part(46));

	//Verificar se a distância verticalmente das extremidades do olho é maior que a flag do valor máximo da distância
	if (valorMaxOlho < aberturaDosOlhos)
		valorMaxOlho = aberturaDosOlhos;
	if (valorMinOlho > aberturaDosOlhos || valorMinOlho == 0)
		valorMinOlho = aberturaDosOlhos;
	//Dados de amostragem
	string s = "IMG-" + std::to_string(x) + ": " + std::to_string(aberturaDosOlhos);
	cout << s << endl;

}
*/
//preencher pontos do rosto
void preencherPontos(cv_image<bgr_pixel> cimg, full_object_detection shape) {
	//extremidades do rosto
	draw_solid_circle(cimg, shape.part(8), 5, bgr_pixel(255, 165, 0));
	draw_solid_circle(cimg, shape.part(25), 5, bgr_pixel(255, 165, 0));
	draw_solid_circle(cimg, shape.part(18), 5, bgr_pixel(255, 165, 0));

	//extremidades da boca
	draw_solid_circle(cimg, shape.part(51), 5, bgr_pixel(0, 0, 255));
	draw_solid_circle(cimg, shape.part(57), 5, bgr_pixel(0, 0, 255));

	//VD = (44 ATÉ 45) / 2 ATÉ (47 ATÉ 48) / 2
	draw_line(cimg, (shape.part(43) + shape.part(44)) / 2,
		(shape.part(46) + shape.part(47)) / 2,
		bgr_pixel(0, 0, 255));

	//HD = 46 ATÉ 43
	draw_line(cimg, shape.part(45), shape.part(42), bgr_pixel(0, 255, 0));

	//extremidades do olho direito
	draw_solid_circle(cimg, shape.part(42), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(43), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(44), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(45), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(46), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(47), 1, bgr_pixel(255, 0, 0));

	//VE = (38 ATÉ 39) / 2 ATÉ (41 ATÉ 42) / 2
	draw_line(cimg, (shape.part(37) + shape.part(38)) / 2,
		(shape.part(40) + shape.part(41)) / 2,
		bgr_pixel(0, 0, 255));

	//HE = 37 ATÉ 40
	draw_line(cimg, shape.part(36), shape.part(39), bgr_pixel(0, 255, 0));

	//extremidades do olho esquerdo
	draw_solid_circle(cimg, shape.part(36), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(37), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(38), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(39), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(40), 1, bgr_pixel(255, 0, 0));
	draw_solid_circle(cimg, shape.part(41), 1, bgr_pixel(255, 0, 0));

	cv::Mat mat = toMat(cimg);
	cv::Mat img;
	dlib::rectangle roiEsquerda(shape.part(36).x() - 20, shape.part(37).y() + 20,
		shape.part(39).x() + 20, shape.part(41).y() - 20);
	cv::Rect roiEsquerdaOpenCV(cv::Point2i(roiEsquerda.left(), roiEsquerda.top()), 
		cv::Point2i(roiEsquerda.right() + 1, roiEsquerda.bottom() + 1));

	img = mat(roiEsquerdaOpenCV);
	cv::imwrite("IMG ROI OLHO ESQUERDO - " + std::to_string(x) + ".png", img);

	dlib::rectangle roiDireita(shape.part(42).x() - 20, shape.part(44).y() + 20,
		shape.part(46).x() + 20, shape.part(45).y() - 20);
	cv::Rect roiDireitaOpenCV(cv::Point2i(roiDireita.left(), roiDireita.top()),
		cv::Point2i(roiDireita.right() + 1, roiDireita.bottom() + 1));

	img = mat(roiDireitaOpenCV);
	cv::imwrite("IMG ROI OLHO DIREITO - " + std::to_string(x) + ".png", img);
}	

int rostoReferencia(full_object_detection shape) {
	int distancia;

	if (shape.part(9)(20) > shape.part(9)(25)) {
		distancia = shape.part(9)(20);
	}
	else {
		distancia = shape.part(9)(25);
	}
	return distancia;
}

void bocaBocejo(full_object_detection shape) {
	int distanciaExtremidadeVerticalBoca;

	distanciaExtremidadeVerticalBoca = shape.part(52)(58);
	//pontoBocaInferior = shape.part(58).y;

	//distanciaExtremidadeVerticalBoca = pontoBocaSuperior - pontoBocaInferior;
	int total = rostoReferencia(shape);
	if (distanciaExtremidadeVerticalBoca / total > 1) {
		cout << "BOCEJO!" << endl;
	}
	else { 
		cout << "NORMAL!" << endl;
	}
}

Mat operadorSobel(Mat frame_gray) {

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	GaussianBlur(frame_gray, frame_gray, Size(3, 3), 0, 0, BORDER_DEFAULT);
	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X
	//Scharr(frame_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT);
	Sobel(frame_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	/// Gradient Y
	//Scharr(frame_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT);
	Sobel(frame_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	/// Total Gradient (approximate)
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, frame_gray);
	//imwrite("sobel3.jpg" ,frame_gray);


	int threshold_value = 25;
	int threshold_type = 1;
	int const max_BINARY_value = 255;
	GaussianBlur(frame_gray, frame_gray, Size(3, 3), 0, 0, BORDER_DEFAULT);
	Mat frameBinarizada;
	//threshold(frame_gray, frameBinarizada, threshold_value, max_BINARY_value,
		//threshold_type);
	Mat mat = frameBinarizada.clone();
	//medianBlur(mat, frameBinarizada, 3);

	return frame_gray;
}

Mat Morphology_Operations(Mat frame_gray) {
	int morph_elem = 0;
	int morph_size = 0;
	int morph_operator = 0;
	int const max_operator = 4;
	int const max_elem = 2;
	int const max_kernel_size = 21;

	// Since MORPH_X : 2,3,4,5 and 6
	int operation = morph_operator + 2;

	Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));

	/// Apply the specified morphology operation
	morphologyEx(frame_gray, frame_gray, operation, element);
	return frame_gray;
}
//22 frames 
void delay(int tempo) {
	sleep_until(system_clock::now() + milliseconds(tempo));
}

//Rodar videos

int main(){
	try{
		int tipoMatriz = CV_8UC3;	

		//variaveis para inicialização do kinect
		openni::Status status;
		openni::VideoStream stream;
		//inicialização do sensor
		status = openni::OpenNI::initialize();
		openni::Device device;
		status = device.open(openni::ANY_DEVICE);
		const openni::SensorInfo *sensorInfo = device.getSensorInfo(tipoDoSensor);
		status = stream.create(device, tipoDoSensor);

		const openni::Array<openni::VideoMode> *videoModes = &sensorInfo->getSupportedVideoModes();
		int maxResolutionX = -1;
		int maxResolutionIndex = 0;

		for (int j = 0; j < videoModes->getSize(); j++) {
			int resolutionX = (*videoModes)[j].getResolutionX();
			if (resolutionX > maxResolutionX) {
				maxResolutionX = resolutionX;
				maxResolutionIndex = j;
			}
		}
		stream.setVideoMode((*videoModes)[maxResolutionIndex]);

		status = stream.start();

		openni::VideoFrameRef frame;
		
		// Carregar detector de face e estimativa dos pontos
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize("C:\\Users\\almer\\OneDrive\\Documents\\Visual Studio 2015\\Projects\\Project1\\shape_predictor_68_face_landmarks.dat") >> pose_model;
		int x, y;
		if (y = 0, y < 3, y++) {
			if (x = 0, x < 200, x++) {
				matrizRA[y][x] = 0;
			}
		}
		x = 0;
		y = 0;
		double fps = 0;
		time_t inicio, fim;
		//Inicialização do processo de aquisição de frames.
		while (true) {
			//variável para monitorar FPS
			int num_frame = 5000;

			for (int i = 0; i < num_frame; i++){
				if (((i + 1) % 60) == 1) {
					time(&inicio);
					string d = "" + std::to_string(inicio);

				}
			status = stream.readFrame(&frame);
			if (frame.isValid()) {
				cv::Mat srcMat(
					frame.getHeight(), frame.getWidth(), tipoMatriz,
					(void *)frame.getData(), frame.getStrideInBytes());

				//applyColorMap(srcMat, srcMat, COLORMAP_PINK);
				//srcMat = operadorSobel(srcMat);
				//GaussianBlur(srcMat, srcMat, Size(3, 3), 0, 0, BORDER_DEFAULT);
				cv_image<bgr_pixel> cimg(srcMat);

				//O artigo de referência faz o teste com uma amostra de 22 frames em 20 seg. que é equivalente a
				//909.09 ms, porém deve considerar que existe um delay natural da máquina no processamento do algorítmo,
				//poratanto foi calculado e o delay ideal deve ser de 830 ms.
				//delay(15); 

				//cv::waitKey(20000);
				//Tirar foto para coletar amostras de testes
				//tirarFoto(srcMat, x, inicio);
				//printf("img");
				
				// Detectar face 
				std::vector<dlib::rectangle> faces = detector(cimg);
				// Buscar pontos para compor o formato do rosto.
				std::vector<full_object_detection> formatoFace;

				for (unsigned long i = 0; i < faces.size(); ++i) {
					full_object_detection shape = pose_model(cimg, faces[i]);
					if (faces.size() == 1) {
						//bocaBocejo(shape);
						if (x <= 199) {
							matrizRA[0][x] = razaoDeAspecto(shape);
							matrizRA[1][x] = desvioPadrao(x);
							matrizRA[2][x] = erroPadrao(x);
							string s = "Valor de " + std::to_string(x) +
								" -> Valor 1: " + std::to_string(matrizRA[0][x]) +
								" -- Valor 2 :" + std::to_string(matrizRA[1][x]) +
								"-- Valor 3 :" + std::to_string(matrizRA[2][x]) ;
							
						}
						else if (x == 200) {
							calibragemAberturaOlhos(matrizRA);
						}
						else {
							//janelaDeAvaliacaoPerclos(porcentagemAberturaDosOlhos(shape, valorMaxOlho, valorMinOlho, x));
							porcentagemAberturaDosOlhos(shape, valorMaxOlho, valorMinOlho, x);
							if (x == 600) {
								Beep(700, 5000);
							}
							preencherPontos(cimg, shape);
						}
					}
					formatoFace.push_back(shape);
				}
				x++;

				
				//resetar a imagem que mostra na execução
				win.clear_overlay();
	
				//Procedimento de monitoramento de FPS 
				
				//capturar informações da matriz de pixels e do formato do rosto já definido
				win.set_image(cimg);
				win.add_overlay(render_face_detections(formatoFace));
			}
			fps = monitoramentoFPS(inicio, i, fps);
			}

			
		}
	}
	catch (serialization_error& e)
	{
		cout << "Erro no arquivo treinado .dat" << endl;
		cout << endl << e.what() << endl;
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
	}
}

//Rodar frames de imagens em png e não videos
/*
int main() {

		while (x <= 198) {
		Mat frame;
		frame = imread("C:/Users/almer/OneDrive/Documents/Iniciação Científica/AMOSTRAGEM 07 - TESTE DE CALIBRAGEM/TESTE 4/IMG-" + std::to_string(x + 1) +".png");
		x++;

		cv_image<bgr_pixel> cimg(frame);
		// Carregar detector de face e estimativa dos pontos
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor pose_model;
		deserialize("C:\\Users\\almer\\OneDrive\\Documents\\Visual Studio 2015\\Projects\\Project1\\shape_predictor_68_face_landmarks.dat") >> pose_model;

		// Detectar face 
		std::vector<dlib::rectangle> faces = detector(cimg);
		// Buscar pontos para compor o formato do rosto.
		std::vector<full_object_detection> formatoFace;

		for (unsigned long i = 0; i < faces.size(); ++i) {
			full_object_detection shape = pose_model(cimg, faces[i]);
			razaoDeAspecto(shape);
			//preencherPontos(cimg, shape);
			//Testar a distância dos pontos verticais dos olhos
			//calibragemAberturaOlhos(shape, x);
			//janelaDeAvaliacaoPerclos(porcentagemAberturaDosOlhos(shape, 10.049875, 4, x));
			formatoFace.push_back(shape);
		}

		//resetar a imagem mostra na janela
		win.clear_overlay();
					
		//capturar informações da matriz de pixels e do formato do rosto já definido
		win.set_image(cimg);
		win.add_overlay(render_face_detections(formatoFace));

		}

		cout << flagValorMaxOlho << endl;
		cout << flagValorMinOlho << endl;
		system("pause");
}
*/

