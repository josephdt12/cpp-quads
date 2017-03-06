# cpp-quads
Takes any image and transforms it into a blockier version of itself.

## How it Works
1. The image is divided into quadrants, with the mean value calculated per quadant.
2. Each quadrant is then divided into quadrants again, with the mean squared error calculated per sub-quadrant.
3. Quadrants with the highest error are divided further, and this continues for the number of iterations provided by the user

## Original
![original](images/lena.jpg)

## Blockified (2,048 iterations)
![blockified](images/2048.jpg)

## Blockified (100,000 iterations)
![blockified more](images/100000.jpg)

## Another example
![apple](images/apple.jpg)

## Blockified (1,024 iterations)
![apple one](images/apple1.jpg)

## Blockified (10,500 iterations)
![apple two](images/apple2.jpg)
