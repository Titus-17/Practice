#include <cv.h>
#include <iostream>
#include <highgui.h>
#include "rect.hpp"
#include "RGB.hpp"

extern const int MAX_X_DEVIATION = 15;
extern const int MAX_Y_DEVIATION = 15;
extern const int DELETE = -10;
extern const bool SEEK_OBJECT = 0;
extern const bool SEEK_BACKGROUND = 1;
extern const int ROW = 15;

rect END[20];

RGB::RGB(int r,int g,int b, int rd, int gd, int bd):R(r),G(g),B(b),R_DIV(rd), G_DIV(gd), B_DIV(bd){}
rect RGB::find_zone_edges(IplImage* image)
{
	int i;
	int k = 0;
	int t = 0;
	point savePoint[image->height*image->width/200];

	for (int y = 0; y < image->height; y++)
	{
		uchar* ptr = (uchar*) (image->imageData + y * image->widthStep);
		for (int x = 0; x < image->width; x++ )
		{	
			if (abs(ptr[3*x] - B) < B_DIV && abs(ptr[3*x+1] - G) < G_DIV && abs(ptr[3*x + 2] - R) < R_DIV) k++;		
			else k = 0;
			if (k > ROW)
			{
				savePoint[t].x = x - ROW;
				savePoint[t].y = y;
				t++;
				break;
			}
       	}
		k = 0;
	}

	for (i = 1; i < t; i++)
	{
		if (savePoint[i].y-savePoint[i-1].y > MAX_Y_DEVIATION)
		{
			savePoint[i].y = savePoint[i-1].y;
		}
	}

	for(int y = 0; y < image->height; y++)
	{
		uchar* ptr = (uchar*) (image->imageData + y * image->widthStep);
		for(int x = image->width - 1; x > 0; x--)
		{	
			if(abs(ptr[3*x] - B) < B_DIV && abs(ptr[3*x + 1] - G) < G_DIV && abs(ptr[3*x + 2] - R) < R_DIV) k++;		
			else k = 0;
			if (k > ROW)
			{
				savePoint[t].x = x + ROW;
				savePoint[t].y = y;
				t++;
				break;
			}		
		}
		k = 0;
	}
	
	for (i = i; i < t; i++)
	{
		if (savePoint[i].y-savePoint[i-1].y > MAX_Y_DEVIATION)
		{
			savePoint[i].y = savePoint[i-1].y;
		}
	}


	int maxX = savePoint[0].x;
	int minX = savePoint[0].x;
	int maxY = savePoint[0].y;
	int minY = savePoint[0].y;
	for (int i = 0; i < t; i++)
	{ 
		if (maxX < savePoint[i].x) maxX = savePoint[i].x; 
		if (minX > savePoint[i].x) minX = savePoint[i].x;
		if (maxY < savePoint[i].y) maxY = savePoint[i].y; 
		if (minY > savePoint[i].y) minY = savePoint[i].y; 
	}

	point leftTop = {minX, minY};
	int width = maxX - minX;
	int height = maxY - minY;

	rect vih(leftTop, width, height);

	return(vih);
}
int RGB::find_objects_edges(IplImage* image)		//Найти объекты, которые отличаются от фона: image - где искать.		
{													//Объект - все, что отличается от фона.
//-------------------------------------------------------------------------------------------------------------------------------
//------------------------------1. Ищем точки, которые лежат на левых и правых границах объектов---------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
	int k = 0;										//Счетчик схожих пикселей в ряду
	int t = 0;										//Счетчик найденных точек правых границ объекта
	int y;
	int x;
	bool switcher = SEEK_OBJECT;					//Переключатель поиска: когда = SEEK_OBJECT (=0), программа ищет объекты; когда =
													//= SEEK_BACKGROUND, программа ищем фон.
	point LeftPoints[image->height*image->width/200];	//Массив для сохранения найденных точек левых границ объектов
	point RightPoints[image->height*image->width/200];	// --//-- правых границ
	for (y = 0; y < image->height; y++)				//Картинка проходится слева направо, сверху вниз: начальная точка (0,0)
	{
													//находится в левом верхнем углу
		uchar* ptr = (uchar*) (image->imageData + y * image->widthStep);	//Получаем информацию о y-той строке пикселей изображения
		for (x = 0; x < image->width; x++)
		{
			if (switcher == SEEK_OBJECT)			//Проверяем, на чем программа остановилась: на фоне или на объекте.
			{										//Если на фоне, то нужно найти объdект (его левую границу) --> SEEK_OBJECT
													//Если на объекте, то нужно найти фон (правую границу объекта) --> SEEK_BACKGROUND
				if (abs(ptr[3*x] - B) > B_DIV || abs(ptr[3*x+1] - G) > G_DIV || abs(ptr[3*x + 2] - R) > R_DIV)
					k++;							//Решение о том, где в данный момент программа находится, принимается путем 
				else k = 0;							//сравнения RGB кода фона и текущего пикселя. Если хотя бы одна кодировка 
				if (k > ROW)						//отличаются больше, чем на MAX_COLOR_DEVIATION, то программа считает эту
				{									//точку потенциальным объектом и увеличивает переменную k на 1 и проверяются
					LeftPoints[t].x = x - ROW;		//следующие точки. Если переменная k достигает значения ROW, то данный объект
					LeftPoints[t].y = y;			//достаточно велик, чтобы являться стикером; k зануляется и программа ищет фон
					switcher = SEEK_BACKGROUND;		//аналогичным образом с условием совпадения цветов фона и пикселя
					k = 0;
				}
			}
			else if (switcher == SEEK_BACKGROUND)
			{
				if ((abs(ptr[3*x] - B) < B_DIV) && (abs(ptr[3*x+1] - G) < G_DIV) && (abs(ptr[3*x + 2] - R) < R_DIV))
					k++;
				else k = 0;
				if (k > ROW)
				{
					RightPoints[t].x = x - ROW;
					RightPoints[t].y = y;
					t++;
					switcher = SEEK_OBJECT;
					k = 0;
				}
				else if (x == image->width)			//Если правая граница находится у самого края изображения
				{									// то x - ROW не сработает, т.к. не выйдет за край изображения и граница задается вручную
					RightPoints[t].x = image->width - 1;
					RightPoints[t].y = y;
					t++;
					switcher = SEEK_OBJECT;
					k = 0;
				}
			}
		}
	}
//-------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------2. Находим близкорасположенные точки-----------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
	point leftSort[15][(image->height)];			//Массивы для сохранения близкорасположенных точек. Первый индекс - номер 
	point rightSort[15][(image->height)];			//стикера, второй - его точки. Точке левой границы соответсвует точка правой
													//границы									
	point testPoint;								//Тестовая точка, с которой будут сравниваться другие точки; по ходу цикла 
	testPoint.x = DELETE;							//будет меняться. Значение DELETE означает, что точка больше не нужна

	int z = -1;										//Счетчик количества найденных стикеров - первый индекс leftSort и rightSort
	int i = 0;
	int j = 0;
	k = 0;											//Индекс точек - второй индекс leftSort и rightSort

	while (j < t)									//Пока не пройдем все элементы массива будем искать тествовые точки
	{
		while (testPoint.x < 1)						//Если тестовая точка < 1 ==> это либо первая итерация, либо прошел цикл 
		{											//for (i = 0; i < t; i++) и для некоторой тестоврой точки были найдены все 
			j++;									//соседние точки. Поэтому ищем новую тестовую точку
			if (LeftPoints[j].x != DELETE)			//Если точка не = DELETE, то это значит что она не проверялась. Принимаем ее		
			{										//за тестовую и удаляем. Т.к. левой точке должна соответсвовать правая, то можно
				testPoint.x = LeftPoints[j].x;		//удалить правую тоже.
				testPoint.y = LeftPoints[j].y;

				LeftPoints[j].x = DELETE;			//Убираем ее из массива
				LeftPoints[j].y = DELETE;
				RightPoints[j].x = DELETE;
				RightPoints[j].y = DELETE;
			}
			if (j == t - 1) break;
		}

		z++;										//Увеличиваем z, чтобы обозначить новый стикер
		for (i = 0; i < t; i++)
		{
			if ((abs(testPoint.y - LeftPoints[i].y) < MAX_Y_DEVIATION) && (abs(testPoint.x - LeftPoints[i].x) < MAX_X_DEVIATION ))
			{										//Ищем соседние точки,у которых столбцы отличаются максимум на MAX_Y_DEVIATION,
													//а строки - максимум на MAX_X_DEVIATION.
				testPoint.x = LeftPoints[i].x;		//Привязываемся к новой точек 
				testPoint.y = LeftPoints[i].y;
	
				leftSort[z][k].x = LeftPoints[i].x;	//Сохраняем ее значение
				leftSort[z][k].y = LeftPoints[i].y;
				rightSort[z][k].x = RightPoints[i].x;
				rightSort[z][k].y = RightPoints[i].y;

				LeftPoints[i].x = DELETE;			//Убираем ее из массива
				LeftPoints[i].y = DELETE;
				RightPoints[i].x = DELETE;
				RightPoints[i].y = DELETE;

				k++;
			}
		}
		if (k < 10) z--;							//Если найдено меньше 5 близкостоящих точек, то считаем что это помеха
		k = 0;										//и уменьшаем z, чтобы перезаписать это место в массиве.
		testPoint.x = DELETE;
		testPoint.y = DELETE;
	}

	z++;											//Из-за того, z обозначает количество найденных объектов, но считается с -1, 
													//то его необходимо увеличить на 1, чтобы не потерять один объект
//-------------------------------------------------------------------------------------------------------------------------------
//-------------------------------3. Формируем из найденных точек обасти - прямоугльники------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------
	for (j = 0; j < z; j++)
	{
		for (i = 0; i < image->height; i++)
		{
			if ((leftSort[j][i].x < 1 && leftSort[j][i].y < 1) || (rightSort[j][i].x < 1 && rightSort[j][i].y < 1))
			{
				leftSort[j][i].x = leftSort[j][0].x;			//Т.к. массив создается на количество точек большее, 
				leftSort[j][i].y = leftSort[j][0].y;			//чем количесвто найденных точек, то лишние точки инициализируются
				rightSort[j][i].x = rightSort[j][0].x;			//нулями, что будет мешать в дальнейшем. Заменим их на значение точки,
				rightSort[j][i].y = rightSort[j][0].y;			//стоящей под индексом 0
			}
		}
	}

	// for (i = 0; i < z; i++)
	// {
	// 	std::cout << "z = " << i << "\n";
	// 	for (j = 0; j < t; j = j + 20)
	// 		std::cout << leftSort[i][j].x << " " << leftSort[i][j].y << "  " << rightSort[i][j].x << " " << rightSort[i][j].y << "\n";
	// 	std::cout << "\n";
	// }


	for (i = 0; i < z; i++)										//Среди всех точек ищем самую левую, самую верхнюю, самую правую и
	{															//самую нижнию, по которым можно сделать прямоугольник и инициализируем
		int maxX = leftSort[i][0].x;							//ими массив прямоугольников
		int minX = leftSort[i][0].x;
		int maxY = leftSort[i][0].y;
		int minY = leftSort[i][0].y;
			
		for (j = 0; j < image->height; j++)
		{
			if (maxX < leftSort[i][j].x) maxX = leftSort[i][j].x;
			if (maxX < rightSort[i][j].x) maxX = rightSort[i][j].x;

			if (minX > leftSort[i][j].x) minX = leftSort[i][j].x;
			if (minX > rightSort[i][j].x) minX = rightSort[i][j].x;

			if (maxY < leftSort[i][j].y) maxY = leftSort[i][j].y;
			if (maxY < rightSort[i][j].y) maxY = rightSort[i][j].y;

			if (minY > leftSort[i][j].y) minY = leftSort[i][j].y;
			if (minY > rightSort[i][j].y) minY = rightSort[i][j].y;
		}
		point leftTop = {minX, minY};
		int width = maxX - minX;
		int height = maxY - minY;

		if(minX < 0 || minY < 0 || width < 1 || height < 1)
		{
			minX = 1;
			minY = 1;
			width = 20;
			height = 20;
		}
		END[i].set_value(minX, minY, width,height);
	}

	for (int j = 0; j < z; j++)
	{
		for (int i = 0; i < t; i++)				//По какой-то совершенно непонятной причине, при многократном использовании этой функции
		{										//массивы могут не освобождаться в памяти и оставлять в себе информацию, которая будет
			leftSort[j][i].x = 0;				//в при следующем вызове этой функции. Обнуление решает эту проблему.
			leftSort[j][i].y = 0;
			rightSort[j][i].x = 0;
			rightSort[j][i].y = 0;
		}
	}

	return z;
}
