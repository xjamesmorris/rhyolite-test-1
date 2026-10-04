#!/usr/bin/env python3
"""Mock Apple II-inspired temperature converter GUI."""

import tkinter as tk
from tkinter import ttk


class AppleIIStyleApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Celsius to Fahrenheit")
        self.geometry("820x620")
        self.configure(bg="#d9d3c3")
        self.resizable(False, False)

        self._build_ui()

    def _build_ui(self):
        outer = tk.Frame(self, bg="#b8b2a5", padx=24, pady=22)
        outer.pack(fill="both", expand=True)

        bezel = tk.Frame(outer, bg="#2d2b2a", padx=18, pady=18)
        bezel.pack(fill="both", expand=True)

        screen = tk.Frame(bezel, bg="#1d3d2f", padx=26, pady=22)
        screen.pack(fill="both", expand=True)

        title = tk.Label(
            screen,
            text="CELSIUS TO FAHRENHEIT",
            fg="#a7ffb8",
            bg="#1d3d2f",
            font=("Courier", 20, "bold"),
            anchor="center",
        )
        title.pack(anchor="center", pady=(0, 18))

        entry_frame = tk.Frame(screen, bg="#1d3d2f")
        entry_frame.pack(fill="x", pady=8)

        tk.Label(
            entry_frame,
            text="ENTER CELSIUS: ",
            fg="#a7ffb8",
            bg="#1d3d2f",
            font=("Courier", 18, "bold"),
        ).pack(side="left")

        self.entry = tk.Entry(
            entry_frame,
            width=12,
            bg="#d7f4db",
            fg="#0d2011",
            font=("Courier", 18, "bold"),
            justify="center",
        )
        self.entry.pack(side="left", padx=8)

        self.result_var = tk.StringVar(value="")
        result_label = tk.Label(
            screen,
            textvariable=self.result_var,
            fg="#a7ffb8",
            bg="#1d3d2f",
            font=("Courier", 18, "bold"),
            justify="left",
            anchor="w",
            wraplength=600,
        )
        result_label.pack(anchor="w", pady=(18, 8))

        buttons = tk.Frame(screen, bg="#1d3d2f")
        buttons.pack(fill="x", pady=(18, 10))

        convert_btn = ttk.Button(buttons, text="CONVERT", command=self.convert)
        convert_btn.pack(side="left", padx=(0, 12))

        clear_btn = ttk.Button(buttons, text="CLEAR", command=self.clear)
        clear_btn.pack(side="left")

        quit_btn = ttk.Button(buttons, text="QUIT", command=self.destroy)
        quit_btn.pack(side="right")

        self.entry.bind("<Return>", lambda event: self.convert())
        self.entry.focus_set()

    def convert(self):
        try:
            celsius = float(self.entry.get())
            fahrenheit = celsius * 9.0 / 5.0 + 32.0
            self.result_var.set(
                f"FAHRENHEIT: {fahrenheit:.2f}°F\nCELSIUS:    {celsius:.2f}°C"
            )
        except ValueError:
            self.result_var.set("INVALID INPUT")

    def clear(self):
        self.entry.delete(0, tk.END)
        self.result_var.set("")


if __name__ == "__main__":
    app = AppleIIStyleApp()
    app.mainloop()
