# cpp-quads
Divides an image into quadrants, creating a blockier version of itself.

## How it Works
1. The image is divided into quadrants, with the mean value calculated per quadrant.
2. Each quadrant is then divided into quadrants again, with the mean squared error calculated per sub-quadrant.
3. Quadrants with the highest error are divided further, and this continues for the number of iterations provided by the user

## Original
![original](images/lena.jpg)

## Blockified (2,048 iterations)
![blockified](images/lena2048.jpg)

## Blockified (100,000 iterations)
![blockified more](images/lena100000.jpg)

## A bird example
![Pretty bird!](images/bird.jpg)

## Blockified (512 iterations)
![Pretty ugly bird!](images/bird512.jpg)

## Blockified (3000 iterations)
![Artistic bird!](images/bird3000.jpg)

## Another example
![apple](images/apple.jpg)

## Blockified (1,024 iterations)
![apple one](images/apple1024.jpg)

## Blockified (10,500 iterations)
![apple two](images/apple10500.jpg)
