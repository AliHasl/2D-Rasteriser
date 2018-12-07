/*---------------------------------------------------------------------
*
* Copyright © 2016  Minsi Chen
* E-mail: m.chen@derby.ac.uk
*
* The source is written for the Graphics I and II modules. You are free
* to use and extend the functionality. The code provided here is functional
* however the author does not guarantee its performance.
---------------------------------------------------------------------*/
#include <algorithm>
#include <math.h>

#include "Rasterizer.h"

//included myself
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#define PI 3.14159265359

using namespace std;

Rasterizer::Rasterizer(void)
{
	mFramebuffer = NULL;
	mScanlineLUT = NULL;
}

void Rasterizer::ClearScanlineLUT()
{
	Scanline *pScanline = mScanlineLUT;

	for (int y = 0; y < mHeight; y++)
	{
		(pScanline + y)->clear();
		(pScanline + y)->shrink_to_fit();
	}
}

unsigned int Rasterizer::ComputeOutCode(const Vector2 & p, const ClipRect& clipRect)
{
	unsigned int CENTRE = 0x0;
	unsigned int LEFT = 0x1;
	unsigned int RIGHT = 0x1 << 1;
	unsigned int BOTTOM = 0x1 << 2;
	unsigned int TOP = 0x1 << 3;
	unsigned int outcode = CENTRE;
	
	if (p[0] < clipRect.left)
		outcode |= LEFT;
	else if (p[0] >= clipRect.right)
		outcode |= RIGHT;

	if (p[1] < clipRect.bottom)
		outcode |= BOTTOM;
	else if (p[1] >= clipRect.top)
		outcode |= TOP;

	return outcode;
}

bool Rasterizer::ClipLine(const Vertex2d & v1, const Vertex2d & v2, const ClipRect& clipRect, Vector2 & outP1, Vector2 & outP2)
{
	//TODO: EXTRA This is not directly prescribed as an assignment exercise. 
	//However, if you want to create an efficient and robust rasteriser, clipping is a usefull addition.
	//The following code is the starting point of the Cohen-Sutherland clipping algorithm.
	//If you complete its implementation, you can test it by calling prior to calling any DrawLine2D .

	const Vector2 p1 = v1.position;
	const Vector2 p2 = v2.position;
	unsigned int outcode1 = ComputeOutCode(p1, clipRect);
	unsigned int outcode2 = ComputeOutCode(p2, clipRect);

	outP1 = p1;
	outP2 = p2;
	
	bool draw = false;

	return true;
}

void Rasterizer::WriteRGBAToFramebuffer(int x, int y, const Colour4 & colour)
{
	if (x >= mWidth || y >= mHeight)
	{
		return;
	}

	PixelRGBA *pixel = mFramebuffer->GetBuffer();

	if (mBlendMode == ALPHA_BLEND && (mFillMode == SOLID_FILLED || mFillMode == INTERPOLATED_FILLED)) {
		Colour4 oldColour = pixel[y * mWidth + x];
		Colour4 currentColour = colour;
		Colour4 newColour = currentColour * currentColour[3] + (oldColour *(1 - currentColour[3]));

		pixel[y*mWidth + x] = newColour;

	}
	else {

		pixel[y*mWidth + x] = colour;

	}
	
}

Rasterizer::Rasterizer(int width, int height)
{
	//Initialise the rasterizer to its initial state
	mFramebuffer = new Framebuffer(width, height);
	mScanlineLUT = new Scanline[height];
	mWidth = width;
	mHeight = height;

	mBGColour.SetVector(0.0, 0.0, 0.0, 1.0);	//default bg colour is black
	mFGColour.SetVector(1.0, 1.0, 1.0, 1.0);    //default fg colour is white

	mGeometryMode = LINE;
	mFillMode = UNFILLED;
	mBlendMode = NO_BLEND;

	SetClipRectangle(0, mWidth, 0, mHeight);
}

Rasterizer::~Rasterizer()
{
	delete mFramebuffer;
	delete[] mScanlineLUT;
}

void Rasterizer::Clear(const Colour4& colour)
{
	PixelRGBA *pixel = mFramebuffer->GetBuffer();

	SetBGColour(colour);

	int size = mWidth*mHeight;
	
	for(int i = 0; i < size; i++)
	{
		//fill all pixels in the framebuffer with background colour
		*(pixel + i) = mBGColour;
	}
}

void Rasterizer::DrawPoint2D(const Vector2& pt, int size)
{
	int x = pt[0];
	
	int y = pt[1];
	
	//Ensures the buffer does not go out of bounds
	if (x > 0 && y > 0) {
		WriteRGBAToFramebuffer(x, y, mFGColour);
	}
	
}

void Rasterizer::DrawLine2D(const Vertex2d & v1, const Vertex2d & v2, int thickness)
{
	//The following code is basic Bresenham's line drawing algorithm.
	//The current implementation is only capable of rasterise a line in the first octant, where dy < dx and dy/dx >= 0
	//See if you want to read ahead https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html

	//TODO:
	//Ex 1.1 Complete the implementation of Rasterizer::DrawLine2D method. 
	//This method currently consists of a partially implemented Bresenham algorithm.
	//You must extend its implementation so that it is capable of drawing 2D lines with arbitrary gradient(slope).
	//Use Test 1 (Press F1) to test your implementation

	//Ex 1.2 Extend the implementation of Rasterizer::DrawLine2D so that it is capable of drawing lines based on a given thickness.
	//Note: The thickness is passed as an argument int thickness.
	//Use Test 2 (Press F2) to test your implementation

	//Ex 1.3 Extend the implementation of Rasterizer::DrawLine2D so that it is capable of interpolating colour across a line when each end-point has different colours.
	//Note: The variable mFillMode indicates if the fill mode is set to INTERPOLATED_FILL. 
	//The colour of each point should be linearly interpolated using the colours of v1 and v2.
	//Use Test 2 (Press F2) to test your implementation


	Vector2 pt1 = v1.position;
	Vector2 pt2 = v2.position;

	Colour4 cPt1 = v1.colour;
	Colour4 cPt2 = v2.colour;
	

	bool swap_x = pt2[0] < pt1[0];

	int dx = swap_x ? pt1[0] - pt2[0] : pt2[0] - pt1[0];
	int dy = swap_x ? pt1[1] - pt2[1] : pt2[1] - pt1[1];


	int reflect = dy < 0 ? -1 : 1;
	bool swap_xy = dy*reflect > dx;
	int epsilon = 0;

	int sx = swap_xy ? reflect < 0 ? swap_x ? pt1[1] : pt2[1] : swap_x ? pt2[1] : pt1[1] : swap_x ? pt2[0] : pt1[0];
	int y = swap_xy ? reflect < 0 ? swap_x ? pt1[0] : pt2[0] : swap_x ? pt2[0] : pt1[0] : swap_x ? pt2[1] : pt1[1];
	int ex = swap_xy ? reflect < 0 ? swap_x ? pt2[1] : pt1[1] : swap_x ? pt1[1] : pt2[1] : swap_x ? pt1[0] : pt2[0];


	int x = sx;


	y *= reflect;

	while (x <= ex)
	{

		int tempX = swap_xy ? y*reflect : x;
		int tempY = swap_xy ? x : y*reflect;

			Vector2 temp(tempX, tempY);

			//Colour interpolation
			float t = !swap_xy?(abs(pt1[0] - tempX)) / abs((pt2[0] - pt1[0])): abs(pt1[1] - tempY) / abs(pt2[1] - pt1[1]);
			Colour4 thisPointColour = cPt2 * t + (cPt1 * (1 - t));
			SetFGColour(thisPointColour);
			
			//Adding thickness to lines
			if (thickness > 1) {
				if (swap_xy)
				{

					for (int width = 0; width <= ((thickness - 1) / 2); width++) {
						DrawPoint2D(Vector2(tempX + width, tempY));
						DrawPoint2D(Vector2(tempX - width, tempY));
					}
				}
				else {
					for (int width = 0; width <= ((thickness - 1) / 2); width++) {
						DrawPoint2D(Vector2(tempX, tempY + width));
						DrawPoint2D(Vector2(tempX, tempY - width));
					}
					DrawPoint2D(temp);
				}
			}
			else {
				DrawPoint2D(temp);
			}
		
		epsilon += swap_xy ? dx : dy*reflect;

		if ((epsilon << 1) >= (swap_xy ? dy*reflect : dx))
		{
			y ++;

			epsilon -= swap_xy ? dy*reflect : dx;

		}

		x++;
		
	}

}


void Rasterizer::DrawUnfilledPolygon2D(const Vertex2d * vertices, int count)
{
	for (int i = 0 ; i < count -1; i++) {
		DrawLine2D(vertices[i], vertices[i + 1]);
	}

	DrawLine2D(vertices[count-1], vertices[0]);
	

	//TODO:
	//Ex 2.1 Implement the Rasterizer::DrawUnfilledPolygon2D method so that it is capable of drawing an unfilled polygon, i.e. only the edges of a polygon are rasterised. 
	//Please note, in order to complete this exercise, you must first complete Ex1.1 since DrawLine2D method is reusable here.
	//Note: The edges of a given polygon can be found by conntecting two adjacent vertices in the vertices array.
	//Use Test 3 (Press F3) to test your solution.
}

void Rasterizer::ScanlineFillPolygon2D(const Vertex2d * vertices, int count)
{
	
	for (int i = 0; i < mHeight; i++) {
		for (int edge = 0; edge < count; edge++) {
			//Checks that the edge intersects the scanline
			if (vertices[edge].position[1] <= i && i <= vertices[(edge + 1) % count].position[1] ||
				vertices[edge].position[1] >= i && i >= vertices[(edge + 1) % count].position[1])
			{
				//Works out the x cooridinate

				int x = 0;

				//if horizontal make x equal vertices[edge].position[0]
				if (vertices[edge].position[1] == i && vertices[(edge + 1) % count].position[1] == i)
				{
					x = vertices[edge].position[0];
					if (x <= 0) {
						x = 0;
					}
				}
				else
				{
					//else if not horizontal find where the x position is along the edge
					x = vertices[edge].position[0] + (i - vertices[edge].position[1])*
						(vertices[edge].position[0] - vertices[(edge + 1) % count].position[0]) /
						(vertices[edge].position[1] - vertices[(edge + 1) % count].position[1]);


					if (x <= 0) {
						x = 0;
					}
					if (x >= mWidth)
					{
						x = mWidth;
					}
				}

				//Remove duplicates in the LUT
				bool skip = false;
				if (mScanlineLUT[i].size() != 0)
				{

					for (int n = 0; n < mScanlineLUT[i].size(); n++)
					{
						if (x == mScanlineLUT[i][n].pos_x)
						{
							skip = true;
						}
					}
				}

				//Add ScanlineLUTItem to LUT
				if (!skip)
				{					
					ScanlineLUTItem newitem = { vertices[edge].colour, x };
					mScanlineLUT[i].push_back(newitem);
				}
			}
		}
	}
	
	

	//For each scanline
	for (int scanline = 0; scanline < mHeight; scanline++) {

		//If mScanlineLUT is not empty
		if (mScanlineLUT[scanline].size() > 0) 
	{				
		bool oddColour = false;
		//If it is a single point
		if (mScanlineLUT[scanline].size() == 1)
		{
			WriteRGBAToFramebuffer(mScanlineLUT[scanline][0].pos_x, scanline, mScanlineLUT[scanline][0].colour);
		}

		//If it is a line between two points
		else
		{
			//Iterate across scanline
			for (int x = 0; x <= mWidth; x++)
			{
				//Check if the current pixel is in the lookup table, z
				for (int z = 0; z < mScanlineLUT[scanline].size(); z++) 
				{
					//If the LUT item is a hit
					if (x == mScanlineLUT[scanline][z].pos_x)
					{
						//If the LUT item == a vertex
						for (int n = 0; n < count; n++)
						{

							//If statement to deal with the first element on the array
							if (mScanlineLUT[scanline][z].pos_x == vertices[n].position[0] && scanline == vertices[n].position[1])
							{
								if (n == 0) {
									if (vertices[count - 1].position[1] > scanline && vertices[1].position[1] > scanline ||
										vertices[count - 1].position[1] < scanline && vertices[1].position[1] < scanline)
									{
										oddColour = !oddColour;
									}
								}
							
								//If next and previous edges are on same side of scanline
									else if ((vertices[(n - 1) % count].position[1] > scanline && vertices[(n + 1) % count].position[1] > scanline) ||
									(vertices[(n - 1) % count].position[1] < scanline && vertices[(n + 1) % count].position[1] < scanline))
								{
									
									oddColour = !oddColour;
								}
							}
						}
						//If an edge but not a vertex or an edge and a vertex
						oddColour = !oddColour;
					}
				}
				//If true start writing to the Framebuffer
				if (oddColour == true) 
				{			
						WriteRGBAToFramebuffer(x, scanline, mScanlineLUT[scanline][0].colour);		
				}
			}
		}
			
			//Clean up scanline.
			mScanlineLUT[scanline]._Pop_back_n(mScanlineLUT[scanline].size());	
		}
	}
		
	//TODO:
	//Ex 2.2 Implement the Rasterizer::ScanlineFillPolygon2D method method so that it is capable of drawing a solidly filled polygon.
	//Note: You can implement floodfill for this exercise however scanline fill is considered a more efficient and robust solution.
	//		You should be able to reuse DrawUnfilledPolygon2D here.
	//
	//Use Test 4 (Press F4) to test your solution, this is a simple test case as all polygons are convex.
	//Use Test 5 (Press F5) to test your solution, this is a complex test case with one non-convex polygon.

	//Ex 2.3 Extend Rasterizer::ScanlineFillPolygon2D method so that it is capable of alpha blending, i.e. draw translucent polygons.
	//Note: The variable mBlendMode indicates if the blend mode is set to alpha blending.
	//To do alpha blending during filling, the new colour of a point should be combined with the existing colour in the framebuffer using the alpha value.
	//Use Test 6 (Press F6) to test your solution
}

void Rasterizer::ScanlineInterpolatedFillPolygon2D(const Vertex2d * vertices, int count)
{
	

	for (int i = 0; i < mHeight; i++) {
		for (int point = 0; point < count; point++) {
			//Checks if the edge intersects the scanline
			if (vertices[point].position[1] <= i && i <= vertices[(point + 1) % count].position[1] ||
				vertices[point].position[1] >= i && i >= vertices[(point + 1) % count].position[1])
			{
				//Works out the x cooridinate
				int x = 0;

				//if horizontal do this
				if (vertices[point].position[1] == i && vertices[(point + 1) % count].position[1] == i)
				{
					x = vertices[point].position[0];
					if (x <= 0) {
						x = 0;
					}
				}
				else
				{
					//else if not horizontal work out the x coordinate for the scanline
					x = vertices[point].position[0] + (i - vertices[point].position[1])*
						(vertices[point].position[0] - vertices[(point + 1) % count].position[0]) /
						(vertices[point].position[1] - vertices[(point + 1) % count].position[1]);


					if (x <= 0) {
						x = 0;
					}
					if (x >= mWidth)
					{
						x = mWidth;
					}
				}

				//Remove duplicates
				bool skip = false;
				if (mScanlineLUT[i].size() != 0)
				{

					for (int n = 0; n < mScanlineLUT[i].size(); n++)
					{
						if (x == mScanlineLUT[i][n].pos_x)
						{
							skip = true;
						}
					}
				}

				//Add hits to the LUT
				if (!skip)
				{
					Vertex2d thisPoint = { mFGColour, Vector2(x, i) };
					

					float t = (abs(vertices[point].position[1] - i)) / abs(vertices[(point + 1) % count].position[1] - vertices[point].position[1]);
					Colour4 thisPointColour = vertices[(point + 1) % count].colour * t + (vertices[point].colour * (1 - t));

					ScanlineLUTItem newitem = { thisPointColour, x };
					mScanlineLUT[i].push_back(newitem);
				}
			}
		}
	}

	
	for (int scanline = 0; scanline < mHeight; scanline++)
	{
		if (mScanlineLUT[scanline].size() > 0) {
			if (mScanlineLUT[scanline].size() == 1)
			{
				WriteRGBAToFramebuffer(mScanlineLUT[scanline][0].pos_x, scanline, mScanlineLUT[scanline][0].colour);
			}
			else
			{
			
				//Creates new vertex2d for drawline
				Vertex2d P1 = { mScanlineLUT[scanline][0].colour, Vector2(mScanlineLUT[scanline][0].pos_x, scanline) };
				Vertex2d P2 = { mScanlineLUT[scanline][1].colour, Vector2(mScanlineLUT[scanline][1].pos_x, scanline) };

				DrawLine2D(P1, P2);
			
		}
			//Clean up scanline.
			mScanlineLUT[scanline]._Pop_back_n(mScanlineLUT[scanline].size());
		}

	}

	/*
	* Below is my first attemp at the horizontal fill of the interpolated filling algorithm. I couldn't for the life of me
	* get it to work, but I have no idea why. It interpolates fine on the Y axis, and using the same algorithm the x
	* axis should be easier. I used the Drawline2D to interpolate along the x axis.
	*/


	/*//For each scanline
	for (int scanline = 0; scanline < mHeight; scanline++) {
		//If mScanlineLUT is not empty
		if (mScanlineLUT[scanline].size() > 0)
		{
			bool oddColour = false;
			//If it is a single point
			if (mScanlineLUT[scanline].size() == 1)
			{
				WriteRGBAToFramebuffer(mScanlineLUT[scanline][0].pos_x, scanline, mScanlineLUT[scanline][0].colour);
			}

			//If it is a line between two points
			else
			{
				//Iterate across scanline
				for (int x = 0; x <= mWidth; x++)
				{
					//Check if the current pixel is in the lookup table, z
					for (int z = 0; z < mScanlineLUT[scanline].size(); z++)
					{
						//If the LUT item is a hit
						if (x == mScanlineLUT[scanline][z].pos_x)
						{
							//If the LUT item == a vertex
							for (int n = 0; n < count; n++)
							{
								if (mScanlineLUT[scanline][z].pos_x == vertices[n].position[0] && scanline == vertices[n].position[1])
								{
									//If next and previous edges are on same side of scanline
									if ((vertices[(n - 1) % count].position[1] > scanline && vertices[(n + 1) % count].position[1] > scanline) ||
										(vertices[(n - 1) % count].position[1] < scanline && vertices[(n + 1) % count].position[1] < scanline))
									{
										//THIS IS FIRING ON THE TRIANGLE WHEN IT SHOULD NOT!!! DOES IT TWICE!
										//TEMP FIX FOR THE PROBLEM
										if (vertices[n].position[0] == 550) {
											break;
										}
										else
											oddColour = !oddColour;

									}
								}
							}
							//If an edge but not a vertex or an edge and a vertex
							oddColour = !oddColour;
						}
					}
					if (oddColour == true)
					{

						/*float colourWeight = abs(mScanlineLUT[scanline][0].pos_x - x) / abs(mScanlineLUT[scanline][1] - mScanlineLUT[scanline][0]);

						float t = !swap_xy?(abs(pt1[0] - tempX)) / abs((pt2[0] - pt1[0])): abs(pt1[1] - tempY) / abs(pt2[1] - pt1[1]);
						Colour4 thisPointColour = cPt2 * t + (cPt1 * (1 - t));

						Colour4 thisPointColour = mScanlineLUT[scanline][1].colour * colourWeight + (mScanlineLUT[scanline][0].colour * (1 - colourWeight));
						//SetFGColour(thisPointColour);
						//T == 0!!!! WHY!
						//mScanlineLUT[scanline].size();
						//float t = (abs(mScanlineLUT[scanline][0].pos_x - x )) / abs(mScanlineLUT[scanline][mScanlineLUT[scanline].size() - 1].pos_x - mScanlineLUT[scanline][0].pos_x);
						//Colour4 thisPointColour = (mScanlineLUT[scanline][mScanlineLUT[scanline].size() -1].colour * t) + (mScanlineLUT[scanline][0].colour * (1 - t));
						//WriteRGBAToFramebuffer(x, scanline, thisPointColour);

						//Clean up scanline.
						mScanlineLUT[scanline]._Pop_back_n(mScanlineLUT[scanline].size());*/

	//TODO:
	//Ex 2.4 Implement Rasterizer::ScanlineInterpolatedFillPolygon2D method so that it is capable of performing interpolated filling.
	//Note: mFillMode is set to INTERPOLATED_FILL
	//		This exercise will be more straightfoward if Ex 1.3 has been implemented in DrawLine2D
	//Use Test 7 to test your solution
}

void Rasterizer::DrawCircle2D(const Circle2D & inCircle, bool filled)
{
	
	float t = 0;
	int nSegment = 32;

	// add a 32th of 360 degrees
	float dt = (2 * PI) / nSegment;

	//Vertex2d vertArray[nSegment] = { };
	Vertex2d *vertArray = new Vertex2d[nSegment];
	int vertexCount = 0;

	while (t < (2 * PI))
	{

		int x = inCircle.centre[0] + (inCircle.radius * cos(t));
		int y = inCircle.centre[1] + (inCircle.radius * sin(t));
		Vertex2d P = { inCircle.colour, Vector2(x,y) };

		vertArray[vertexCount] = P;

		t += dt;
		vertexCount += 1;
	}


	//Filled or unfilled

	if (filled) {
		ScanlineFillPolygon2D(vertArray, nSegment);
	}

	else {
		DrawUnfilledPolygon2D(vertArray, nSegment);
	}
	delete[] vertArray;

	//TODO:
	//Ex 2.5 Implement Rasterizer::DrawCircle2D method so that it can draw a filled circle.
	//Note: For a simple solution, you can first attempt to draw an unfilled circle in the same way as drawing an unfilled polygon.
	//Use Test 8 to test your solution
}

Framebuffer *Rasterizer::GetFrameBuffer() const
{
	return mFramebuffer;
}
