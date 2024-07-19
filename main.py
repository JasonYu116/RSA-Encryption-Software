from functools import partial
import sys
from tkinter import *
from tkinter import filedialog
from ctypes import CDLL

cMain = CDLL("./main.so")
cKeygen = CDLL("./keygen.so")
fontSize = 40
buttonHeight = 5
buttonWidth = 40

class Tooltip:
    def __init__(self, widget, text):
        self.widget = widget
        self.text = text
        self.tooltip = None
        self.widget.bind("<Enter>", self.show_tooltip)
        self.widget.bind("<Leave>", self.hide_tooltip)

    def show_tooltip(self, event):
        x, y, _, _ = self.widget.bbox("insert")
        x += self.widget.winfo_rootx()
        y += self.widget.winfo_rooty() + buttonHeight * 19 + 1

        self.tooltip = Toplevel(self.widget)
        self.tooltip.wm_overrideredirect(True)
        self.tooltip.wm_geometry(f"+{x}+{y}")

        label = Label(self.tooltip, text=self.text, font=("Times New Roman", int(fontSize/4)), background="pink", relief="raised", borderwidth=2, height=int(buttonHeight), width=int(buttonWidth/5*6) + 1)
        label.pack()

    def hide_tooltip(self, event):
        if self.tooltip:
            self.tooltip.destroy()

class App():
    def __init__(self):
        self.root = Tk()
        self.root.geometry("1400x900")
        self.root.title("RSA Encryption Program")
        self.bgColor = "cyan"
        self.mainframe = Frame(self.root, background=self.bgColor)
        self.mainframe.pack(fill = "both", expand = True)
        # Initial State of Buttons
        self.encrypt = True
        # Identifies if the file and key have been inputted
        self.file = "" 
        self.pubfile = ""
        self.privfile = ""
        self.keyFile = ""
        self.outfile = "encrypted.txt"
        # Title
        self.text = Label(self.mainframe, text="RSA Encryption Program", justify="center", background=self.bgColor, font=("Comic Sans MS", fontSize))
        self.text.place(relx=0.5, rely=0.075, anchor=CENTER)
        # Jason's Instructions
        self.instruction = "Steps to encrypt and decrypt file are as follows: \n 1. Generate public and private keys through the \"Generate a Key\" button. \n 2. Add the file you want to encrypt through \"Add File\". \n 3. Add .pub and .priv files to add a public key or private key. (There's a button for that!) \n 4. Click on \"Encrypt\" and choose where to encrypt the file to and what to name it. \n 5. Toggle to decrypt and choose what file to decrypt and what name and extension that you want your decrypted file to be. \nImportnat Note: Ensure, that decrypted file has the same extension as original file or else decryption will not work.\n(i.e. if you expect the decrypted file to be a zip file make sure to add the .zip extension to the file name [fileName].zip)\nFurther details can be found by hovering over the buttons."
        self.text2 = Label(self.mainframe, text=self.instruction, justify="left", background=self.bgColor, font=("Times New Roman", int(fontSize/3)))
        self.text2.place(relx=0.5, rely=0.22, anchor=CENTER)
        # Add File Button
        self.buttonOpenFile = Button(self.mainframe, text="Add File", height=buttonHeight, width=buttonWidth, command=partial(self.chooseFile, self.encrypt, False))
        self.buttonOpenFile.place(relx=0.3, rely=0.4, anchor=CENTER)
        Tooltip(self.buttonOpenFile, "Add a file for encryption or decryption respectively.")
        # Add Key Button
        self.buttonOpenKey = Button(self.mainframe, text="Add Public Key" if self.encrypt else "Add Private Key", height=buttonHeight, width=buttonWidth, command = partial(self.chooseFile, self.encrypt, True))
        self.buttonOpenKey.place(relx=0.7, rely=0.4, anchor=CENTER)
        Tooltip(self.buttonOpenKey, "Adds a .pub or .priv file to public or private key.\n Ensure that only said specified files are added.")
        # File Label
        self.label = Label(self.mainframe, text="No File Selected", background=self.bgColor)
        self.label.place(relx=0.5, rely=0.5, anchor=CENTER)
        # Key File Label
        self.label2 = Label(self.mainframe, text="No Key File Selected", background=self.bgColor)
        self.label2.place(relx=0.5, rely=0.55, anchor=CENTER)
        # Keygen Button - Calls Keygen Function
        self.buttonKeygen = Button(self.mainframe, text="Generate A Key", height=buttonHeight, width=buttonWidth, command=self.callToCKeygen)
        self.buttonKeygen.place(relx=0.3, rely=0.65, anchor=CENTER)
        Tooltip(self.buttonKeygen, "Generates .pub and .priv for public and private\n keys respectively. Only one generation necessary for\n all encrypts and decrypts.")
        # Toggle Button - Toggles Encrypt Function
        self.buttonToggle = Button(self.mainframe, text="Toggle", height=buttonHeight, width=buttonWidth, command=self.toggleButton)
        self.buttonToggle.place(relx=0.7, rely=0.65, anchor=CENTER)
        Tooltip(self.buttonToggle, "Toggle between encrypt or decrypt. Ensure that\n .pub is for public and .priv for private.")
        # Encrypt Button - Calls Main Function
        self.buttonEncrypt = Button(self.mainframe, text="Encrypt" if self.encrypt else "Decrypt", height=buttonHeight, width=buttonWidth, command = partial(self.callToCMain, self.encrypt, self.file, self.outfile, self.keyFile))
        self.buttonEncrypt.place(relx=0.5, rely=0.8, anchor=CENTER)
        self.buttonEncrypt.place_forget()
        Tooltip(self.buttonEncrypt, "Encrypts user designated file with public key. \n Cancel/Exit option generates default naming convention." if self.encrypt else "Decrypts User File with private key. \n Cancel/exit options generates default naming convention.")
        self.labelSuccess = Label(self.mainframe, text="The file has been generated", background=self.bgColor)
        self.labelSuccess.place(relx=0.5, rely=0.9, anchor=CENTER)
        self.labelSuccess.place_forget()
        self.root.mainloop()
        return
    # Encrypt means you're encrypting a file and key specifies if the file is a keyfile or not.
    def chooseFile(self, encrypt, key):
        if encrypt:
            if key:
                self.keyFile = filedialog.askopenfilename(title="Select A Public Key File", filetypes=(("Public Key Files", "*.pub"),))
                self.pubfile = self.keyFile
            else:
                self.file = filedialog.askopenfilename(title="Select A File To Encrypt", filetypes=(("Files", "*.*"),))
        else:
            if key:
                self.keyFile = filedialog.askopenfilename(title="Select A Private Key File", filetypes=(("Private Key Files", "*.priv"),))
                self.privfile = self.keyFile
            else:
                self.file = filedialog.askopenfilename(title="Select A File To Decrypt", filetypes=(("Files", "*.*"),))
        if self.file != "":
            self.label.config(text='File: "%s"' % self.file)
        if self.keyFile != "":
            self.label2.config(text='KeyFile: "%s"' % self.keyFile)
        if self.file != "" and self.keyFile != "":
            self.buttonEncrypt.config(text="Encrypt" if self.encrypt else "Decrypt", command=partial(self.callToCMain, self.encrypt, self.file, self.outfile, self.keyFile))
            self.buttonEncrypt.place(relx=0.5, rely=0.8, anchor=CENTER)
        return
    def toggleButton(self):
        self.encrypt = not self.encrypt
        self.keyFile = self.pubfile if self.encrypt else self.privfile
        self.buttonOpenFile.config(command=partial(self.chooseFile, self.encrypt, False))
        self.buttonOpenKey.config(text="Add Public Key" if self.encrypt else "Add Private Key", command = partial(self.chooseFile, self.encrypt, True))
        self.buttonEncrypt.config(text="Encrypt" if self.encrypt else "Decrypt", command=partial(self.callToCMain, self.encrypt, self.file, self.outfile, self.keyFile))
        self.label2.config(text='KeyFile: "%s"' % self.keyFile)
        if self.file == "" or self.keyFile == "":
            self.buttonEncrypt.place_forget()
        elif self.file != "" or self.keyFile != "":
            self.buttonEncrypt.place(relx=0.5, rely=0.8, anchor=CENTER)
        return
    def callToCMain(self, encrypt, file, out, key):
        out = filedialog.asksaveasfile(defaultextension=".txt")
        out = self.outfile if (out == None) else out.name
        failed = cMain.mainFunc(encrypt, file.encode(), out.encode(), key.encode())
        self.labelSuccess.place(relx=0.5, rely=0.9, anchor=CENTER)
        self.labelSuccess.config(text="Error: File not generated correctly." if failed else ("You have successfully " + ("encrypted" if encrypt else "decrypted") + " the file \"" + file + "\" into \"" + out + "\"."))
        self.file = ""
        self.label.config(text="No File Selected")
        self.buttonEncrypt.place_forget()
        return
    def callToCKeygen(self):
        failed = cKeygen.keygen()
        self.labelSuccess.place(relx=0.5, rely=0.9, anchor=CENTER)
        self.labelSuccess.config(text="Key generation of public and private keys " + ("successful." if not failed else "failed."))
        return

if __name__ == "__main__":
    App()
