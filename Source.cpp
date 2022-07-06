#include <iostream>
#include <string>
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\videoio.hpp>
#include <math.h>

#define PI 3.142857

extern "C" {
#include "vc.h"
}

float Convertor(int valor) {
	return valor / 5.01;
}

int Perimetro(int raio) {
	return raio * 2 * PI;
}
int main(void) {
	// V�deo
	char videofile[20] = "video.avi";
	cv::VideoCapture capture;
	struct
	{
		int width, height;
		int ntotalframes;
		int fps;
		int nframe;
	} video;
	// Outros
	std::string str;
	int key = 0;
	OVC* blobs;
	int nblobs = 0;
	int laranja = 1;
	int area = 0;
	int showCalibre = 0;
	int c[100000], calibres = 0;
	int limiteInf;
	std::string calibre;


	/* Leitura de v�deo de um ficheiro */
	/* NOTA IMPORTANTE:
	O ficheiro video.avi dever� estar localizado no mesmo direct�rio que o ficheiro de c�digo fonte.
	*/
	capture.open(videofile);

	/* Em alternativa, abrir captura de v�deo pela Webcam #0 */
	//capture.open(0, cv::CAP_DSHOW); // Pode-se utilizar apenas capture.open(0);

	/* Verifica se foi poss�vel abrir o ficheiro de v�deo */
	if (!capture.isOpened())
	{
		std::cerr << "Erro ao abrir o ficheiro de v�deo!\n";
		return 1;
	}
	/* N�mero total de frames no v�deo */
	video.ntotalframes = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
	/* Frame rate do v�deo */
	video.fps = (int)capture.get(cv::CAP_PROP_FPS);
	/* Resolu��o do v�deo */
	video.width = (int)capture.get(cv::CAP_PROP_FRAME_WIDTH);
	video.height = (int)capture.get(cv::CAP_PROP_FRAME_HEIGHT);

	/* Cria uma janela para exibir o v�deo */
	cv::namedWindow("VC - VIDEO", cv::WINDOW_AUTOSIZE);

	cv::Mat frame;
	while (key != 'q') {
		/* Leitura de uma frame do v�deo */
		capture.read(frame);

		/* Verifica se conseguiu ler a frame */
		if (frame.empty()) break;

		/* N�mero da frame a processar */
		video.nframe = (int)capture.get(cv::CAP_PROP_POS_FRAMES);

		/* Exemplo de inser��o texto na frame */

		str = std::string("RESOLUCAO: ").append(std::to_string(video.width)).append("x").append(std::to_string(video.height));
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("TOTAL DE FRAMES: ").append(std::to_string(video.ntotalframes));
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("FRAME RATE: ").append(std::to_string(video.fps));
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 75), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DA FRAME: ").append(std::to_string(video.nframe));
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
		str = std::string("N. DE LARANJAS: ").append(std::to_string(laranja));
		cv::putText(frame, str, cv::Point(20, 125), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 0), 2);
		cv::putText(frame, str, cv::Point(20, 125), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);

		// Fa�a o seu c�digo aqui...

		IVC* image = vc_image_new(video.width, video.height, 3, 255);
		IVC* image_dst = vc_image_new(image->width, image->height, 3, 255);
		IVC* image_dst1 = vc_image_new(image->width, image->height, 1, 255);
		IVC* image_dst2 = vc_image_new(image->width, image->height, 1, 255);
		IVC* image_dst3 = vc_image_new(image->width, image->height, 1, 255);
		IVC* image_original = vc_image_new(video.width, video.height, 3, 255);

		memcpy(image_original->data, frame.data, video.width * video.height * 3);
		memcpy(image->data, frame.data, video.width * video.height * 3);
		vc_bgr_to_rgb(image);
		vc_rgb_to_hsv(image, image_dst);
		vc_hsv_segmentation(image_dst, image_dst1, 15, 50, 40, 100, 30, 100);
		//vc_binary_dilate(image_dst1, image_dst2, 3);
		vc_binary_open(image_dst1, image_dst2, 4, 4);
		blobs = vc_binary_blob_labelling(image_dst2, image_dst3, &nblobs);
		vc_binary_blob_info(image_dst3, blobs, nblobs);

		calibres = 0;
		int raio = 0;

		for (int i = 0; i < nblobs; i++)
		{
			int xr = blobs[i].xc - blobs[i].x;
			int yr = blobs[i].yc - blobs[i].y;

			if (xr > yr)
			{
				raio = xr;
			}
			else
			{
				raio = yr;
			}
			if (blobs[i].area > 70000 && blobs[i].yc < 150 && blobs[i].yc > 100)
			{
				int xr = blobs[i].xc - blobs[i].x;
				int yr = blobs[i].yc - blobs[i].y;

				if (xr > yr)
				{
					raio = xr;
				}
				else
				{
					raio = yr;
				}

				if (blobs[i].area < area)
				{
					laranja++;
				}

				area = blobs[i].area;
			}

			if (blobs[i].area > 70000) {
				limiteInf = blobs[i].yc + raio + 20;
				int xr = blobs[i].xc - blobs[i].x;
				int yr = blobs[i].yc - blobs[i].y;
				if (xr > yr)
				{
					raio = xr;
				}
				else
				{
					raio = yr;
				}
				cv::circle(frame, cv::Point(blobs[i].xc, blobs[i].yc), 6, cv::Scalar(0, 0, 255, 0), 4, 4, 0);
				cv::circle(frame, cv::Point(blobs[i].xc, blobs[i].yc), raio, cv::Scalar(0, 0, 255, 0), 4, 2, 0);

				if (Convertor(raio * 2) >= 53 && Convertor(raio * 2) <= 60)
				{
					calibre.append(" 13");
					calibres++;
					c[calibres] = 13;
				}
				if (Convertor(raio * 2) >= 56 && Convertor(raio * 2) <= 63)
				{
					calibre.append(" 12");
					calibres++;
					c[calibres] = 12;
				}
				if (Convertor(raio * 2) >= 58 && Convertor(raio * 2) <= 66)
				{
					calibre.append(" 11");
					calibres++;
					c[calibres] = 11;
				}
				if (Convertor(raio * 2) >= 60 && Convertor(raio * 2) <= 68)
				{
					calibre.append(" 10");
					calibres++;
					c[calibres] = 10;
				}
				if (Convertor(raio * 2) >= 62 && Convertor(raio * 2) <= 70)
				{
					calibre.append(" 9");
					calibres++;
					c[calibres] = 9;
				}
				if (Convertor(raio * 2) >= 64 && Convertor(raio * 2) <= 73)
				{
					calibre.append(" 8");
					calibres++;
					c[calibres] = 8;
				}
				if (Convertor(raio * 2) >= 67 && Convertor(raio * 2) <= 76)
				{
					calibre.append(" 7");
					calibres++;
					c[calibres] = 7;
				}
				if (Convertor(raio * 2) >= 70 && Convertor(raio * 2) <= 80)
				{
					calibre.append(" 6");
					calibres++;
					c[calibres] = 6;
				}
				if (Convertor(raio * 2) >= 73 && Convertor(raio * 2) <= 84)
				{
					calibre.append(" 5");
					calibres++;
					c[calibres] = 5;
				}
				if (Convertor(raio * 2) >= 77 && Convertor(raio * 2) <= 88)
				{
					calibre.append(" 4");
					calibres++;
					c[calibres] = 4;
				}
				if (Convertor(raio * 2) >= 81 && Convertor(raio * 2) <= 92)
				{
					calibre.append(" 3");
					calibres++;
					c[calibres] = 3;
				}
				if (Convertor(raio * 2) >= 84 && Convertor(raio * 2) <= 96)
				{
					calibre.append(" 2");
					calibres++;
					c[calibres] = 2;
				}
				if (Convertor(raio * 2) >= 87 && Convertor(raio * 2) <= 100)
				{
					calibre.append(" 1");
					calibres++;
					c[calibres] = 1;
				}
				if (Convertor(raio * 2) >= 100)
				{
					calibre.append(" 0");
					calibres++;
					c[calibres] = 0;
				}
				str = std::string("Calibre:").append(calibre);
				cv::putText(frame, str, cv::Point(blobs[i].xc, limiteInf), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
				str = std::string("Area: ").append(std::to_string(int(Convertor(area))));
				cv::putText(frame, str, cv::Point(blobs[i].xc, limiteInf - 25), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
				str = std::string("Perimetro: ").append(std::to_string(int(Convertor(blobs[i].perimeter))));
				cv::putText(frame, str, cv::Point(blobs[i].xc, limiteInf - 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 1);
				calibre.clear();
			}
		}
		//memcpy(frame.data, image_dst->data, video.width * video.height * 3); //-> HSV
		//memcpy(frame.data, image_dst1->data, video.width * video.height * 1); //-> Segmentaçao
		//memcpy(frame.data, image_dst2->data, video.width * video.height * 1); //->Dilataçao
		vc_image_free(image_original);

		// +++++++++++++++++++++++++

		/* Exibe a frame */
		cv::imshow("VC - VIDEO", frame);

		/* Sai da aplica��o, se o utilizador premir a tecla 'q' */
		key = cv::waitKey(1);
	}
	/* Fecha a janela */
	cv::destroyWindow("VC - VIDEO");

	/* Fecha o ficheiro de v�deo */
	capture.release();

	return 0;
}