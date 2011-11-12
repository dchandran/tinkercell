import window

def runTests():
    "just create a window with some colored panels"
    test = window.Window()
    test.p1 = window.Window.ControlPanel(test, bg="green")
    test.p1.pack(fill=window.BOTH, expand=True)
    buttonNames = ("Close", "Open", "Whatever")
    test.p2 = window.Window.ButtonPanel(test, buttonNames, bg="blue")
    test.p2.buttons[0].config(command=test.destroy)
    test.p2.pack(side=window.BOTTOM, fill=window.X, expand=True)
    window.Label(test, text="this is a label").pack()
    window.Entry(test).pack()
    test.mainloop()

if __name__ == "__main__":
    runTests()

