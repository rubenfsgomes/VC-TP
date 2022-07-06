//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//           INSTITUTO POLIT?CNICO DO C?VADO E DO AVE
//                          2011/2012
//             ENGENHARIA DE SISTEMAS INFORM?TICOS
//                    VIS?O POR COMPUTADOR
//
//             [  DUARTE DUQUE - dduque@ipca.pt  ]
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define MAX(a, b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

#define VC_DEBUG


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UMA IMAGEM
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


typedef struct {
	unsigned char* data;
	int width, height;
	int channels;			// Bin?rio/Cinzentos=1; RGB=3
	int levels;				// Bin?rio=1; Cinzentos [1,255]; RGB [1,255]
	int bytesperline;		// width * channels
} IVC;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROT?TIPOS DE FUN??ES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int vc_rgb_to_hsv(IVC* src, IVC* dst);

int vc_bgr_to_rgb(IVC* srcdst);

int vc_hsv_segmentation(IVC* src, IVC* dst, int hmin, int hmax, int smin, int smax, int vmin, int vmax);

int vc_binary_dilate(IVC* src, IVC* dst, int kernel);

int vc_binary_erode(IVC* src, IVC* dst, int kernel);

int vc_binary_open(IVC* src, IVC* dst, int kernel1, int kernel2);

// FUN??ES: ALOCAR E LIBERTAR UMA IMAGEM
IVC* vc_image_new(int width, int height, int channels, int levels);
IVC* vc_image_free(IVC* image);

// FUN??ES: LEITURA E ESCRITA DE IMAGENS (PBM, PGM E PPM)
IVC* vc_read_image(char* filename);
int vc_write_image(char* filename, IVC* image);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                   ESTRUTURA DE UM BLOB (OBJECTO)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct {
	int x, y, width, height;	// Caixa Delimitadora (Bounding Box)
	int area;					// Área
	int xc, yc;					// Centro-de-massa
	int perimeter;				// Perímetro
	int label;					// Etiqueta
} OVC;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                    PROTÓTIPOS DE FUNÇÕES
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

OVC* vc_binary_blob_labelling(IVC* src, IVC* dst, int* nlabels);
int vc_binary_blob_info(IVC* src, OVC* blobs, int nblobs);