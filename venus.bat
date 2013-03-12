SET pathfile=C:\Users\Yohann\Documents\GitHub\ADCensus

SET execfile=%pathfile%\ADCensus_Build\bin\Release\stereoDense.exe

SET img=Venus
SET img1=%pathfile%\Images\%img%\left_picture.png
SET img2=%pathfile%\Images\%img%\right_picture.png

SET dmin=-20
SET dmax=0

SET dmap=%pathfile%\Images\%img%\disparity_map.png

SET cmd=%execfile% -O r %img1% %img2% %dmin% %dmax% %dmap%

start %cmd%