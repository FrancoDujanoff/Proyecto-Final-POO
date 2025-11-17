import tkinter as tk
from tkinter import ttk, messagebox, filedialog
from GestorArchivosLocal import GestorArchivosLocal

class VistaGUI(tk.Tk):
    def __init__(self, cliente_rpc):
        super().__init__()
        self.cliente = cliente_rpc
        self.gestor_archivos = GestorArchivosLocal()
        
        # Estados locales para los botones "Toggle"
        self.estado_pinza = False
        self.estado_ventilador = False

        self.title("Mando de Control - Robot RRR")
        self.geometry("900x600")
        self.configure(bg="#2c3e50") # Fondo oscuro profesional

        self.construir_login()

    def construir_login(self):
        """Pantalla de Login simple"""
        self.frame_login = tk.Frame(self, bg="#34495e", padx=20, pady=20)
        self.frame_login.place(relx=0.5, rely=0.5, anchor="center")

        tk.Label(self.frame_login, text="ACCESO AL SISTEMA", font=("Arial", 16, "bold"), bg="#34495e", fg="white").pack(pady=10)
        
        self.entry_user = tk.Entry(self.frame_login, font=("Arial", 12)); self.entry_user.pack(pady=5)
        self.entry_user.insert(0, "admin") # Autocompletar para facilitar pruebas
        
        self.entry_pass = tk.Entry(self.frame_login, show="*", font=("Arial", 12)); self.entry_pass.pack(pady=5)
        
        tk.Button(self.frame_login, text="CONECTAR", command=self.accion_login, bg="#27ae60", fg="white", font=("Arial", 12, "bold"), width=20).pack(pady=20)

    def accion_login(self):
        user = self.entry_user.get()
        pwd = self.entry_pass.get()
        self.cliente.set_credenciales(user, pwd)
        
        res = self.cliente.intentar_login()
        if "Exitoso" in res:
            self.frame_login.destroy()
            self.construir_dashboard()
        else:
            messagebox.showerror("Error", res)

    def construir_dashboard(self):
        """Construye la interfaz tipo Panel de Control"""
        
        # --- 1. PANTALLA CENTRAL (Consola) ---
        frame_centro = tk.Frame(self, bg="#2c3e50")
        frame_centro.place(relx=0.3, rely=0.05, relwidth=0.4, relheight=0.8)
        
        tk.Label(frame_centro, text="TERMINAL DE SALIDA", fg="#ecf0f1", bg="#2c3e50", font=("Consolas", 10)).pack(anchor="w")
        
        self.consola = tk.Text(frame_centro, bg="black", fg="#00ff00", font=("Consolas", 10), state="disabled")
        self.consola.pack(expand=True, fill="both", padx=5, pady=5)

        # --- 2. PANEL IZQUIERDO (Movimiento y Conexión) ---
        frame_izq = tk.Frame(self, bg="#ecf0f1", bd=2, relief=tk.RIDGE)
        frame_izq.place(relx=0.02, rely=0.05, relwidth=0.26, relheight=0.8)

        tk.Label(frame_izq, text="CINEMÁTICA", font=("Arial", 12, "bold"), bg="#ecf0f1").pack(pady=10)

        # Conexión
        tk.Button(frame_izq, text="🔌 CONECTAR SERIE", bg="#95a5a6", command=lambda: self.ejecutar_y_log(self.cliente.conectar_hardware, True)).pack(fill="x", padx=10, pady=2)
        tk.Button(frame_izq, text="⚡ HABILITAR MOTORES", bg="#f1c40f", command=lambda: self.ejecutar_y_log(self.cliente.controlar_motores, True)).pack(fill="x", padx=10, pady=2)
        tk.Button(frame_izq, text="🏠 HOME (G28)", bg="#e67e22", fg="white", command=lambda: self.ejecutar_y_log(self.cliente.ir_a_home)).pack(fill="x", padx=10, pady=10)

        # Inputs de Coordenadas
        frm_coords = tk.Frame(frame_izq, bg="#ecf0f1")
        frm_coords.pack(pady=10)
        
        self.ent_x = self.crear_input(frm_coords, "X:", 0)
        self.ent_y = self.crear_input(frm_coords, "Y:", 1)
        self.ent_z = self.crear_input(frm_coords, "Z:", 2)
        self.ent_vel = self.crear_input(frm_coords, "F:", 3, "10.0")

        tk.Button(frame_izq, text="▶ MOVER ROBOT", bg="#3498db", fg="white", font=("Arial", 12, "bold"), command=self.accion_mover).pack(fill="x", padx=20, pady=20)

    
    # --- 3. PANEL DERECHO (Accesorios y Modos) ---
        frame_der = tk.Frame(self, bg="#ecf0f1", bd=2, relief=tk.RIDGE)
        frame_der.place(relx=0.72, rely=0.05, relwidth=0.26, relheight=0.8)

        tk.Label(frame_der, text="HERRAMIENTAS", font=("Arial", 12, "bold"), bg="#ecf0f1").pack(pady=10)

        # Botón Toggle PINZA
        self.btn_pinza = tk.Button(frame_der, text="PINZA: CERRADA", bg="#e74c3c", fg="white", height=2, command=self.toggle_pinza)
        self.btn_pinza.pack(fill="x", padx=10, pady=5)

        # Botón Toggle VENTILADOR
        self.btn_fan = tk.Button(frame_der, text="VENTILADOR: OFF", bg="#e74c3c", fg="white", height=2, command=self.toggle_ventilador)
        self.btn_fan.pack(fill="x", padx=10, pady=5)

        tk.Label(frame_der, text="----------------", bg="#ecf0f1").pack(pady=5)

        # Modos
        tk.Label(frame_der, text="MODO COORDENADAS", bg="#ecf0f1", font=("Arial", 8)).pack()
        frm_modos = tk.Frame(frame_der, bg="#ecf0f1")
        frm_modos.pack(pady=5)
        tk.Button(frm_modos, text="Absoluto (G90)", command=lambda: self.ejecutar_y_log(self.cliente.set_modo_coordenadas, True)).pack(side=tk.LEFT)
        tk.Button(frm_modos, text="Relativo (G91)", command=lambda: self.ejecutar_y_log(self.cliente.set_modo_coordenadas, False)).pack(side=tk.LEFT)

        tk.Button(frame_der, text="🔍 CONSULTAR ENDSTOPS", bg="#9b59b6", fg="white", command=lambda: self.ejecutar_y_log(self.cliente.reporte_endstops)).pack(fill="x", padx=10, pady=20)


        # --- 4. BARRA INFERIOR (Archivos y Reportes) ---
        frame_bot = tk.Frame(self, bg="#34495e", height=80)
        frame_bot.place(relx=0, rely=0.88, relwidth=1.0, relheight=0.12)

        tk.Button(frame_bot, text="📂 Subir Archivo G-Code", command=self.accion_subir, width=20).pack(side=tk.LEFT, padx=20, pady=15)
        tk.Button(frame_bot, text="🚀 Ejecutar Archivo Remoto", command=self.accion_ejecutar_archivo, width=20).pack(side=tk.LEFT, padx=5, pady=15)
        
        # Espaciador
        tk.Label(frame_bot, text="", bg="#34495e", width=10).pack(side=tk.LEFT)

        tk.Button(frame_bot, text="📄 Generar Reporte Estado", command=lambda: self.ejecutar_y_log(self.cliente.obtener_reporte), width=25).pack(side=tk.RIGHT, padx=20, pady=15)


    # --- LOGICA DE INTERFAZ ---

    def crear_input(self, parent, label, row, default=""):
        tk.Label(parent, text=label, bg="#ecf0f1", font=("Arial", 10, "bold")).grid(row=row, column=0, padx=5, pady=5)
        entry = tk.Entry(parent, width=8, font=("Arial", 10))
        entry.grid(row=row, column=1, padx=5, pady=5)
        if default: entry.insert(0, default)
        return entry

    def log_console(self, mensaje):
        """Escribe en la pantalla negra del centro"""
        self.consola.config(state="normal")
        self.consola.insert(tk.END, f">> {mensaje}\n")
        self.consola.see(tk.END) # Auto-scroll
        self.consola.config(state="disabled")

    def ejecutar_y_log(self, funcion, *args):
        """Helper para ejecutar comandos RPC y mostrar resultado en consola"""
        self.log_console("Enviando comando...")
        try:
            res = funcion(*args)
            self.log_console(f"RESPUESTA: {res}")
            self.log_console("-" * 30)
        except Exception as e:
            self.log_console(f"ERROR: {str(e)}")

    # --- LÓGICA DE BOTONES TOGGLE (CAMBIO DE COLOR) ---

    def toggle_pinza(self):
        # Invertimos estado
        nuevo_estado = not self.estado_pinza
        
        # Ejecutamos comando (True=Abrir/Activar, False=Cerrar/Desactivar)
        # Ajusta según tu lógica de hardware: M3 suele ser abrir o cerrar según el robot.
        res = self.cliente.controlar_efector(nuevo_estado)
        
        if "ERROR" not in str(res):
            self.estado_pinza = nuevo_estado
            if self.estado_pinza:
                self.btn_pinza.config(text="PINZA: ABIERTA", bg="#27ae60") # Verde
            else:
                self.btn_pinza.config(text="PINZA: CERRADA", bg="#e74c3c") # Rojo
            self.log_console(f"PINZA: {res}")
        else:
            self.log_console(f"ERROR PINZA: {res}")

    def toggle_ventilador(self):
        nuevo_estado = not self.estado_ventilador
        res = self.cliente.controlar_ventilador(nuevo_estado)
        
        if "ERROR" not in str(res):
            self.estado_ventilador = nuevo_estado
            if self.estado_ventilador:
                self.btn_fan.config(text="VENTILADOR: ON", bg="#27ae60") # Verde
            else:
                self.btn_fan.config(text="VENTILADOR: OFF", bg="#e74c3c") # Rojo
            self.log_console(f"FAN: {res}")
        else:
            self.log_console(f"ERROR FAN: {res}")

    # --- ACCIONES COMPLEJAS ---

    def accion_mover(self):
        try:
            x = self.ent_x.get()
            y = self.ent_y.get()
            z = self.ent_z.get()
            v = self.ent_vel.get()
            
            self.log_console(f"Moviendo a X:{x} Y:{y} Z:{z} F:{v}")
            res = self.cliente.mover_lineal(x, y, z, v)
            self.log_console(res)
        except Exception as e:
            messagebox.showerror("Error", "Valores inválidos")

    def accion_subir(self):
        ruta = filedialog.askopenfilename(filetypes=[("G-Code", "*.gcode"), ("Texto", "*.txt")])
        if ruta:
            nombre, contenido = self.gestor_archivos.leer_archivo(ruta)
            if nombre:
                self.log_console(f"Subiendo archivo: {nombre}...")
                res = self.cliente.subir_archivo_gcode(nombre, contenido)
                self.log_console(res)

    def accion_ejecutar_archivo(self):
        # Pedimos el nombre simple (una mejora sería listar primero y dejar elegir)
        nombre = tk.simpledialog.askstring("Ejecutar", "Nombre del archivo en servidor (ej. prueba.gcode):")
        if nombre:
            self.log_console(f"Solicitando ejecución de {nombre}...")
            res = self.cliente.ejecutar_archivo_remoto(nombre)
            self.log_console(res)