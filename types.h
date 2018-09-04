typedef struct tagPALETTEENTRY {
    unsigned char        peRed;
    unsigned char        peGreen;
    unsigned char        peBlue;
    unsigned char        peFlags;
} PALETTEENTRY;

typedef struct {
	unsigned char atari_x,atari_y;	//��??
	unsigned short hit;				// �ɷ����� //��???
	short height;				//?????
	short broken;				//????
	short indamage;				//�VHP????
	short outdamage;			//��????
	short inpoison;				//�V��
	short innumb;				//�V???
	short inquiet;				//�V��?
	short instone;				//�V��?
	short indark;				//�V??
	short inconfuse;			//�V??
	short outpoison;			//�ҥ�
	short outnumb;				//��??
	short outquiet;				//�ң�?
	short outstone;				//����?
	short outdark;				//��??
	short outconfuse;			//��??
	short effect1;				//?????1??��????????��?????
	short effect2;				//?????2?�V???????��?????
	unsigned short damy_a;
	unsigned short damy_b;
	unsigned short damy_c;
	unsigned int bmpnumber;		//??�k?
} MAP_ATTR;


struct ADRNBIN{
	unsigned int	bitmapno;
	unsigned int	adder;
	unsigned int	size;
	int	xoffset;
	int	yoffset;
	unsigned int width;
	unsigned int height;

	MAP_ATTR attr;

};