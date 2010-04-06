from tinkercell import *

def numbers(itemNames, numbers,adjustLine=True,adjustColors=True,minLineWidth=0.1,maxLineWidth=10.0):
    items = tc_findItems( toStrings(itemNames) );
    items = fromItems( items );
    n = len(numbers);
    if (len(itemNames) < n):
        n = len(itemNames);
    if n < 1:
        return;
    maxN = numbers[0];
    minN = numbers[0];
    for i in range(0,n):
        if minN > abs(numbers[i]):
            minN = abs(numbers[i]);
        if maxN < abs(numbers[i]):
            maxN = abs(numbers[i]);
    ratio1 = (maxLineWidth - minLineWidth)/(maxN - minN);
    ratio2 = 1.0/(maxN - minN);
    for i in range(0,n):
        lineWidth = minLineWidth + numbers[i] * ratio1;
        r = 0;
        g = 0;
        b = 0;
        if numbers[i] < 0: 
            r = 240;#int( 10 + 255 * (abs(numbers[i]) - minN) * ratio2 );
        elif numbers[i] > 0: 
            g = 240;#int( 10 + 255 * (abs(numbers[i]) - minN) * ratio2 );
        else:
            b = 100;
        if not items[i] == 0:
            tc_displayNumber(items[i],numbers[i]);
            if adjustColors:
                tc_setColor(items[i],toHex(r,g,b),0);
            if adjustLine:
               tc_setLineWidth(items[i],0.1 + lineWidth,0);

def text(itemNames, text):
    items = tc_findItems( toStrings(itemNames) );
    n = len(text);
    if (len(itemNames) < n):
        n = len(itemNames);
    for i in range(0,items.length):
        if not nthItem(items,i) == 0:
            tc_displayText( nthItem(items,i) ,text[i]);

