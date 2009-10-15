import pytc

def numbers(itemNames, numbers,adjustLine=True,adjustColor=True,minLineWidth=0.1,maxLineWidth=10.0):
    items = pytc.find(itemNames);
    n = len(numbers);
    if (len(itemNames) < n):
        n = len(itemNames);
    if n < 1:
	    return;
    maxN = numbers[0];
    minN = numbers[0];
    for i in range(0,n):
        if minN > numbers[i]:
            minN = numbers[i];
        if maxN < numbers[i]:
            maxN = numbers[i];
    lineWidths = range(0,n);
    lineColorsR = range(0,n);
    lineColorsG = range(0,n);
    ratio1 = (maxLineWidth - minLineWidth)/(maxN - minN);
    ratio2 = 1.0/(maxN - minN);
    for i in range(0,n):
        lineWidths = minLineWidth + numbers[i] * ratio1;
        lineColorsR = (numbers[i] * ratio2) * 250.0;
        lineColorsG = (1.0 - numbers[i] * ratio2) * 250.0;
    for i in range(0,len(items)):
        if not items[i] == 0:
            pytc.displayNumber(items[i],numbers[i]);
            if adjustLine:
                pytc.setLineWidth(items[i],lineWidths[i]);
        if adjustColors:
            pytc.setColor(items[i],lineColorsR[i],lineColorsG[i],0.0);

def text(itemNames, text):
    items = pytc.find(itemNames);
    n = len(numbers);
    if (len(itemNames) < n):
        n = len(itemNames);
    for i in items:
        if not items[i] == 0:
            pytc.displayText(items[i],text[i]);

