import pytc

def numbers(itemNames, numbers,adjustLine=True,adjustColors=True,minLineWidth=0.1,maxLineWidth=10.0):
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
    ratio1 = (maxLineWidth - minLineWidth)/(maxN - minN);
    ratio2 = 1.0/(maxN - minN);
    for i in range(0,n):
        lineWidth = minLineWidth + numbers[i] * ratio1;
        lineColorR = (numbers[i] * ratio2) * 250.0;
        lineColorG = (1.0 - numbers[i] * ratio2) * 250.0;
        if not items[i] == 0:
            pytc.displayNumber(items[i],numbers[i]);
            if adjustColors:
                pytc.setColor(items[i],lineColorR,lineColorG,0.0);
            if adjustLine:
               pytc.setLineWidth(items[i],lineWidth);

def text(itemNames, text):
    items = pytc.find(itemNames);
    n = len(numbers);
    if (len(itemNames) < n):
        n = len(itemNames);
    for i in items:
        if not items[i] == 0:
            pytc.displayText(items[i],text[i]);

