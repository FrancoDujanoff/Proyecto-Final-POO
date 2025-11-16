"""
Interfaz de Escritorio (Operator) usando Tkinter.
Controles: conectar/desconectar, enviar G-Code, pedir estado, pedir logs.
"""
import tkinter as tk
from tkinter import ttk, messagebox, scrolledtext, simpledialog
from desktop_ui.rpc_adapter import RPCAdapter


class _MoverDialog(tk.Toplevel):
    """Diálogo emergente para ingresar coordenadas X Y Z."""
    def __init__(self, parent):
        super().__init__(parent)
        self.title("Mover a Coordenadas")
        self.geometry("300x150")
        self.result = None

        ttk.Label(self, text="X:").grid(row=0, column=0, sticky=tk.W, padx=8, pady=8)
        self.x_var = tk.StringVar(value="0")
        ttk.Entry(self, textvariable=self.x_var, width=15).grid(row=0, column=1, padx=8, pady=8)

        ttk.Label(self, text="Y:").grid(row=1, column=0, sticky=tk.W, padx=8, pady=8)
        self.y_var = tk.StringVar(value="0")
        ttk.Entry(self, textvariable=self.y_var, width=15).grid(row=1, column=1, padx=8, pady=8)

        ttk.Label(self, text="Z:").grid(row=2, column=0, sticky=tk.W, padx=8, pady=8)
        self.z_var = tk.StringVar(value="0")
        ttk.Entry(self, textvariable=self.z_var, width=15).grid(row=2, column=1, padx=8, pady=8)

        btn_frame = ttk.Frame(self)
        btn_frame.grid(row=3, column=0, columnspan=2, pady=8)

        ttk.Button(btn_frame, text="OK", command=self._ok).pack(side=tk.LEFT, padx=4)
        ttk.Button(btn_frame, text="Cancelar", command=self._cancel).pack(side=tk.LEFT, padx=4)

        self.transient(parent)
        self.grab_set()

    def _ok(self):
        try:
            x = float(self.x_var.get())
            y = float(self.y_var.get())
            z = float(self.z_var.get())
            self.result = (x, y, z)
            self.destroy()
        except ValueError:
            messagebox.showerror("Error", "Ingresa números válidos para X Y Z")

    def _cancel(self):
        self.result = None
        self.destroy()


class OperatorUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Operator - Robot Remote")
        self.geometry("700x520")

        # RPC adapter
        self.rpc = RPCAdapter()

        # Top frame: server config + auth
        top = ttk.Frame(self)
        top.pack(fill=tk.X, padx=8, pady=8)

        ttk.Label(top, text="Host:").grid(row=0, column=0, sticky=tk.W)
        self.host_var = tk.StringVar(value=self.rpc.host)
        ttk.Entry(top, textvariable=self.host_var, width=15).grid(row=0, column=1)

        ttk.Label(top, text="Port:").grid(row=0, column=2, sticky=tk.W, padx=(8,0))
        self.port_var = tk.StringVar(value=str(self.rpc.port))
        ttk.Entry(top, textvariable=self.port_var, width=7).grid(row=0, column=3)

        ttk.Label(top, text="Usuario:").grid(row=0, column=4, sticky=tk.W, padx=(8,0))
        self.user_var = tk.StringVar(value="admin")
        ttk.Entry(top, textvariable=self.user_var, width=12).grid(row=0, column=5)

        ttk.Label(top, text="Clave:").grid(row=0, column=6, sticky=tk.W, padx=(8,0))
        self.pwd_var = tk.StringVar(value="1234")
        ttk.Entry(top, textvariable=self.pwd_var, width=12, show='*').grid(row=0, column=7)

        self.connect_btn = ttk.Button(top, text="Conectar", command=self.connect)
        self.connect_btn.grid(row=0, column=8, padx=(8,0))

        self.disconnect_btn = ttk.Button(top, text="Desconectar", command=self.disconnect)
        self.disconnect_btn.grid(row=0, column=9, padx=(4,0))

        # Token label
        self.token_label = ttk.Label(self, text="Token: (no conectado)")
        self.token_label.pack(fill=tk.X, padx=8)

        # Middle frame: commands
        mid = ttk.Frame(self)
        mid.pack(fill=tk.X, padx=8, pady=(6,0))

        ttk.Label(mid, text="Comando G-Code:").grid(row=0, column=0, sticky=tk.W)
        self.cmd_entry = ttk.Entry(mid, width=50)
        self.cmd_entry.grid(row=0, column=1, padx=(6,0))
        ttk.Button(mid, text="Enviar", command=self.send_gcode).grid(row=0, column=2, padx=(6,0))

        ttk.Button(mid, text="Estado", command=self.get_estado).grid(row=1, column=1, sticky=tk.W, pady=(8,0))
        ttk.Button(mid, text="Logs (10)", command=lambda: self.get_logs(10)).grid(row=1, column=1, padx=(90,0), sticky=tk.W, pady=(8,0))
        ttk.Button(mid, text="Logs (50)", command=lambda: self.get_logs(50)).grid(row=1, column=1, padx=(170,0), sticky=tk.W, pady=(8,0))

        # Botones adicionales (fila siguiente)
        adv = ttk.Frame(self)
        adv.pack(fill=tk.X, padx=8, pady=(4,0))
        ttk.Button(adv, text="Home (G28)", command=self.home).pack(side=tk.LEFT, padx=(0,4))
        ttk.Button(adv, text="Mover (XYZ)", command=self.mover).pack(side=tk.LEFT, padx=(0,4))
        ttk.Button(adv, text="Activar Motors (M17)", command=self.activate_motors).pack(side=tk.LEFT, padx=(0,4))

        # Output area
        out_label = ttk.Label(self, text="Salida / Logs")
        out_label.pack(anchor=tk.W, padx=8, pady=(8,0))

        self.output = scrolledtext.ScrolledText(self, height=18)
        self.output.pack(fill=tk.BOTH, expand=True, padx=8, pady=(0,8))

    def connect(self):
        host = self.host_var.get().strip() or 'localhost'
        port = int(self.port_var.get().strip() or 8000)
        usuario = self.user_var.get().strip()
        clave = self.pwd_var.get().strip()

        try:
            self.rpc.set_server(host, port)
            ok = self.rpc.conectar(usuario, clave)
            if ok:
                self.token_label.config(text=f"Token: {self.rpc.token}")
                self._log(f"Conectado como {usuario}. Token: {self.rpc.token}")
            else:
                messagebox.showerror("Conexión", "No se pudo conectar")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def disconnect(self):
        try:
            self.rpc.desconectar()
            self.token_label.config(text="Token: (no conectado)")
            self._log("Desconectado")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def send_gcode(self):
        cmd = self.cmd_entry.get().strip()
        if not cmd:
            return
        try:
            resp = self.rpc.enviar_gcode(cmd)
            self._log(f"> {cmd}\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def get_estado(self):
        try:
            estado = self.rpc.obtener_estado()
            self._log(f"ESTADO: {estado}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def get_logs(self, n=10):
        try:
            logs = self.rpc.obtener_logs(n)
            self._log("Logs:\n" + "\n".join(logs) + "\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def home(self):
        """Envía comando G28 (home)."""
        try:
            resp = self.rpc.enviar_gcode("G28")
            self._log(f"> G28\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def mover(self):
        """Abre diálogo para ingresar coordenadas X Y Z y envía G01."""
        try:
            d = _MoverDialog(self)
            if d.result:
                x, y, z = d.result
                cmd = f"G01 X{x} Y{y} Z{z}"
                resp = self.rpc.enviar_gcode(cmd)
                self._log(f"> {cmd}\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def activate_motors(self):
        """Envía comando M17 (activar motores)."""
        try:
            resp = self.rpc.enviar_gcode("M17")
            self._log(f"> M17\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def _log(self, text: str):
        self.output.insert(tk.END, text + "\n")
        self.output.see(tk.END)


def run():
    app = OperatorUI()
    app.mainloop()


if __name__ == '__main__':
    run()


def build_operator_frame(parent, rpc_adapter: RPCAdapter = None):
    """Construye y devuelve un `ttk.Frame` con los controles del Operator.

    Esta función permite que la UI sea creada desde código (por ejemplo, por otra
    aplicación) sin depender de crear directamente un `Tk` desde aquí.

    - `parent`: widget padre (por ejemplo `tk.Tk()` o `tk.Toplevel()`)
    - `rpc_adapter`: instancia opcional de `RPCAdapter`. Si es `None` se crea una nueva.
    """
    if rpc_adapter is None:
        rpc_adapter = RPCAdapter()

    frame = ttk.Frame(parent)

    # Host / Port / Auth row
    top = ttk.Frame(frame)
    top.pack(fill=tk.X, padx=8, pady=8)

    ttk.Label(top, text="Host:").grid(row=0, column=0, sticky=tk.W)
    host_var = tk.StringVar(value=rpc_adapter.host)
    ttk.Entry(top, textvariable=host_var, width=15).grid(row=0, column=1)

    ttk.Label(top, text="Port:").grid(row=0, column=2, sticky=tk.W, padx=(8,0))
    port_var = tk.StringVar(value=str(rpc_adapter.port))
    ttk.Entry(top, textvariable=port_var, width=7).grid(row=0, column=3)

    ttk.Label(top, text="Usuario:").grid(row=0, column=4, sticky=tk.W, padx=(8,0))
    user_var = tk.StringVar(value="admin")
    ttk.Entry(top, textvariable=user_var, width=12).grid(row=0, column=5)

    ttk.Label(top, text="Clave:").grid(row=0, column=6, sticky=tk.W, padx=(8,0))
    pwd_var = tk.StringVar(value="1234")
    ttk.Entry(top, textvariable=pwd_var, width=12, show='*').grid(row=0, column=7)

    token_label = ttk.Label(frame, text="Token: (no conectado)")
    token_label.pack(fill=tk.X, padx=8)

    # Middle frame: commands
    mid = ttk.Frame(frame)
    mid.pack(fill=tk.X, padx=8, pady=(6,0))

    ttk.Label(mid, text="Comando G-Code:").grid(row=0, column=0, sticky=tk.W)
    cmd_entry = ttk.Entry(mid, width=50)
    cmd_entry.grid(row=0, column=1, padx=(6,0))

    def _log(text: str):
        output.insert(tk.END, text + "\n")
        output.see(tk.END)

    def connect_cb():
        host = host_var.get().strip() or 'localhost'
        port = int(port_var.get().strip() or 8000)
        usuario = user_var.get().strip()
        clave = pwd_var.get().strip()
        try:
            rpc_adapter.set_server(host, port)
            ok = rpc_adapter.conectar(usuario, clave)
            if ok:
                token_label.config(text=f"Token: {rpc_adapter.token}")
                _log(f"Conectado como {usuario}. Token: {rpc_adapter.token}")
            else:
                messagebox.showerror("Conexión", "No se pudo conectar")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def disconnect_cb():
        try:
            rpc_adapter.desconectar()
            token_label.config(text="Token: (no conectado)")
            _log("Desconectado")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def send_gcode_cb():
        cmd = cmd_entry.get().strip()
        if not cmd:
            return
        try:
            resp = rpc_adapter.enviar_gcode(cmd)
            _log(f"> {cmd}\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def get_estado_cb():
        try:
            estado = rpc_adapter.obtener_estado()
            _log(f"ESTADO: {estado}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def get_logs_cb(n=10):
        try:
            logs = rpc_adapter.obtener_logs(n)
            _log("Logs:\n" + "\n".join(logs) + "\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    ttk.Button(mid, text="Enviar", command=send_gcode_cb).grid(row=0, column=2, padx=(6,0))
    ttk.Button(mid, text="Estado", command=get_estado_cb).grid(row=1, column=1, sticky=tk.W, pady=(8,0))
    ttk.Button(mid, text="Logs (10)", command=lambda: get_logs_cb(10)).grid(row=1, column=1, padx=(90,0), sticky=tk.W, pady=(8,0))
    ttk.Button(mid, text="Logs (50)", command=lambda: get_logs_cb(50)).grid(row=1, column=1, padx=(170,0), sticky=tk.W, pady=(8,0))

    # Botones avanzados
    def home_cb():
        try:
            resp = rpc_adapter.enviar_gcode("G28")
            _log(f"> G28\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def mover_cb():
        try:
            d = _MoverDialog(parent)
            if d.result:
                x, y, z = d.result
                cmd = f"G01 X{x} Y{y} Z{z}"
                resp = rpc_adapter.enviar_gcode(cmd)
                _log(f"> {cmd}\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    def activate_motors_cb():
        try:
            resp = rpc_adapter.enviar_gcode("M17")
            _log(f"> M17\n< {resp}\n")
        except PermissionError:
            messagebox.showwarning("No conectado", "Conecta primero al servidor")
        except Exception as e:
            messagebox.showerror("Error", str(e))

    adv = ttk.Frame(frame)
    adv.pack(fill=tk.X, padx=8, pady=(4,0))
    ttk.Button(adv, text="Home (G28)", command=home_cb).pack(side=tk.LEFT, padx=(0,4))
    ttk.Button(adv, text="Mover (XYZ)", command=mover_cb).pack(side=tk.LEFT, padx=(0,4))
    ttk.Button(adv, text="Activar Motors (M17)", command=activate_motors_cb).pack(side=tk.LEFT, padx=(0,4))

    # Buttons row
    connect_btn = ttk.Button(top, text="Conectar", command=connect_cb)
    connect_btn.grid(row=0, column=8, padx=(8,0))
    disconnect_btn = ttk.Button(top, text="Desconectar", command=disconnect_cb)
    disconnect_btn.grid(row=0, column=9, padx=(4,0))

    # Output area
    out_label = ttk.Label(frame, text="Salida / Logs")
    out_label.pack(anchor=tk.W, padx=8, pady=(8,0))

    output = scrolledtext.ScrolledText(frame, height=18)
    output.pack(fill=tk.BOTH, expand=True, padx=8, pady=(0,8))

    return frame


def create_operator_window(rpc_adapter: RPCAdapter = None):
    """Crea una ventana `Tk` con la UI y la devuelve. Llamar `.mainloop()`.

    Ejemplo:
      win = create_operator_window()
      win.mainloop()
    """
    root = tk.Tk()
    root.title("Operator - Robot Remote")
    root.geometry("700x520")
    frame = build_operator_frame(root, rpc_adapter=rpc_adapter)
    frame.pack(fill=tk.BOTH, expand=True)
    return root
