'---- GENERAL SCREEN COMMANDS AND EFFECTS ----


'-- FADE IN SCREEN (12-BIT COLOR MODE ONLY) --

SUB FADE_IN_12(PNGIMAGE$)

 LOCAL T

 'FIRST TURNS SCREEN BLACK
 PAGE WRITE 1: BOX 0,0,MM.HRES,MM.VRES,0,,RGB(0,0,0,15)

 'LOAD IMAGE
 PAGE WRITE 0
 IF LEN(PNGIMAGE$)>2 THEN LOAD PNG "./IMAGES/"+PNGIMAGE$+".PNG"

 'FADE IN
 PAGE WRITE 1
 FOR T=15 TO 0 STEP -1
  PAUSE(50)
  BOX 0,0,MM.HRES,MM.VRES,0,,RGB(0,0,0,T)
 NEXT T

END SUB


'-- FADE OUT SCREEN (12-BIT COLOR MODE ONLY) --

SUB FADE_OUT_12

 LOCAL T

 PAGE COPY 0 TO 2,I
 PAGE WRITE 2
 BLIT 0,0,0,0,MM.HRES,MM.VRES,1,4
 PAGE COPY 2 TO 1,I
 PAGE COPY 2 TO 0,I

 PAGE WRITE 1
 FOR T=0 TO 15
  PAUSE(50)
  BOX 0,0,MM.HRES,MM.VRES,0,,RGB(0,0,0,T)
 NEXT T

END SUB


'-- PRINT STATUS BAR NUMBERS --

SUB SB_PRINT(X,Y,NUM,MASK)
 
 LOCAL T,A$

 SELECT CASE MASK
  CASE 1: 
   A$=RIGHT$("000"+STR$(NUM),3)
  CASE 3: A$=RIGHT$("000"+STR$(NUM),3)
  CASE 6: A$=RIGHT$("000000"+STR$(NUM),6)
  CASE ELSE:
   A$=STR$(NUM)
   IF NUM>9 THEN X=X-4
   IF NUM>99 THEN X=X-4
 END SELECT

 FOR T=1 TO LEN(A$)+1
  BLIT (PEEK(VAR A$,T)-48)*8,93, X+T*8-8,Y, 8,15, 13,0
 NEXT T

END SUB



'-- PRINT STAGE CLEAR FONT --

SUB SC_PRINT(X,Y,B$,MASK)

 LOCAL T,I

 A$=RIGHT$(SPACE$(MASK)+B$,MASK)
 
 FOR T=0 TO MASK-1
  I=(PEEK(VAR A$,(T+1))-48)
  BOX X+T*16,Y,16,16,0,,RGB(0,&H40,&H40)
  IF I>=0 AND I<=9 THEN BLIT I*17,0, X+T*16,Y, 16,16, 12,0
 NEXT T

END SUB



'-- ANIMATE BJ HEAD FOLLOWING THE HEALTH --

SUB BJ_HEAD(EN,HAPPY)

 T=((1000-EN*10)\165)

 IF HEADTIME<=0 THEN
  HEADMV=0
  IF RND(1)>0.9 THEN HEADMV=23: HEADTIME=5
  IF RND(1)<0.1 THEN HEADMV=-23: HEADTIME=5
 ELSE
  HEADTIME=HEADTIME-1
 ENDIF
 XH=T*69:YH=0
 IF XH>207 THEN XH=XH-207:YH=YH+31
 IF XH>138 THEN XH=XH-138:YH=YH+31: XH=0
 IF EN<=1 THEN HEADMV=0

 IF HAPPY=0 THEN
  BLIT XH+23+HEADMV,YH,137,165, 23,31, 13,0
 ELSE
  'HAPPY FACE
  BLIT 0,62,137,165, 23,31, 13,0
 ENDIF

END SUB


'-- DRAW CELLING AND FLOOR BOXES ON MAIN BUFFER PAGE

SUB FLOOR_CELLING(RC,GC,BC,RF,GF,BF)

 PAGE WRITE 1
 BOX 8,4,MM.HRES-16,MM.VRES/2-21-WX_SIZE/2,0,,RGB(RC,GC,BC) 'CELLING
 BOX 8,MM.VRES/2-20-WX_SIZE/2,MM.HRES-16,76,0,,RGB(RF,GF,BF) 'FLOOR

END SUB


'-- RECALCULATE AND REFRESH THE SCREEN SIZE AND DETAIL

SUB SCREEN_DETAIL(WX,DETAIL)

 LOCAL T

 'SIZE OF SLICES OF RAYCASTER (LEVEL OF DETAIL)
 IF DETAIL>0 THEN SIZE=DETAIL ELSE SIZE=4

 WX_SIZE=WX

 PAGE WRITE 0
 
 'SCREEN FRAME
 IF ZOOM=1 THEN T=WX_SIZE: WX_SIZE=0
 BOX 7,3,MM.HRES-14,MM.VRES-46,0,,RGB(&H00,&H40,&H40)
 BOX 8+WX_SIZE,4+WX_SIZE/2,MM.HRES-16-WX_SIZE*2,MM.VRES-48-WX_SIZE,0,,RGB(0,0,0)
 LINE 7+WX_SIZE,3+WX_SIZE/2, 7+WX_SIZE,MM.VRES-44-WX_SIZE/2, 1,RGB(&H2C,&H2C,&H2C)
 LINE 7+WX_SIZE,3+WX_SIZE/2, MM.HRES-8-WX_SIZE,3+WX_SIZE/2, 1,RGB(&H2C,&H2C,&H2C)
 LINE MM.HRES-8-WX_SIZE,3+WX_SIZE/2, MM.HRES-8-WX_SIZE,MM.VRES-44-WX_SIZE/2,  1,RGB(&H00,&H70,&H70)
 LINE 7+WX_SIZE,MM.VRES-44-WX_SIZE/2, MM.HRES-8-WX_SIZE,MM.VRES-44-WX_SIZE/2, 1,RGB(&H00,&H70,&H70)
 PIXEL 7+WX_SIZE,MM.VRES-44-WX_SIZE/2, RGB(&H00,&H84,&H84)
 IF ZOOM=1 THEN WX_SIZE=T

 'RAYCAST SCAN RANGE
 A_SCAN=6+WX_SIZE
 B_SCAN=MM.HRES-6-WX_SIZE

 'RAYCAST SCAN SIZE
 X_DIM = (B_SCAN - A_SCAN)/SIZE

 'LIMIT WINDOW
 BX_DIM = X_DIM\2
 W_DIM  = MM.HRES/2
 Y_DIM  = MM.VRES-44-WX_SIZE
 B_Y_DIM = Y_DIM\2
 IF WX_SIZE >0 THEN F_DIST=Y_DIM/(MM.VRES-44) ELSE F_DIST=1

 'ZOOM OUT CURRENT WEAPON WHEN ZOOM WINDOW IS NOT SET
 IF ZOOM=0 THEN CHANGE_WEAPON(WEAPON)

 'SET Z-BUFFER TO CUT ANY OBJECT OUTSIDE THE WINDOW
 FOR T=0 TO 320
  Z_OBJ(T)=MM.VRES
 NEXT T

 'DRAW FLOOR AND CELLING BEFORE MAP BEFORE ENTERING THE LEVEL
 FLOOR_CELLING(56,56,56,113,113,113)

END SUB


'-- CHANGE AND ZOOM CURRENT WEAPON --

SUB CHANGE_WEAPON(W)

 LOCAL X,Y,T

 PAGE WRITE 0
 BOX 249,165,61,31,0,,RGB(0,0,&HA4)

 SELECT CASE W
  CASE 0: X=  0: Y= 0: BLIT 277,22,257,174,43,13,13 'KNIFE
  CASE 1: X=128: Y= 0: BLIT 276, 0,260,172,38,18,13 'PISTOL
  CASE 2: X=  0: Y=32: BLIT 100,93,257,174,44,15,13 'MACHINE GUN
  CASE 3: X=128: Y=32: BLIT 144,93,255,172,47,20,13 'CHAIN GUN
 END SELECT

 WEAPON=W

 'ZOOM CURRENT WEAPON
 IF ZOOM=0 THEN T=F_DIST ELSE T=1 
 PAGE WRITE 12
 IMAGE RESIZE_FAST X,Y,128,32,0,64,256*T,64*T
 PAGE WRITE 1

END SUB


'-- DRAW THE LOADING BAR IN THE GAME START (GET PSYCHED!) --

SUB PSYCHED_BAR(P)

 IF P>100 THEN P=100 
 PAGE WRITE 0
 BOX 53,99,(214/100)*P,1,1,RGB(255,0,0),RGB(255,0,0)

END SUB


