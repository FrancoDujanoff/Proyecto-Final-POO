"""
Interfaz de Escritorio (Admin) usando Tkinter.
Permite listar, crear y eliminar usuarios en `server/usuarios.db` (crear_local).
"""
import tkinter as tk
from tkinter import ttk, messagebox
from pathlib import Path


class GestorUsuariosLocal:
    def __init__(self, repo_root: Path = None):
        if repo_root is None:
            repo_root = Path(__file__).resolve().parents[2]
        self.file = repo_root / 'server' / 'usuarios.db'
        self.users = {}
        self._load()

    def _load(self):
        self.users = {}
        if not self.file.exists():
            return
        for line in self.file.read_text(encoding='utf-8').splitlines():
            line = line.strip()
            if not line:
                continue
            parts = line.split(',')
            if len(parts) >= 3:
                username, password, adm = parts[0], parts[1], parts[2]
                self.users[username] = {'password': password, 'admin': adm == '1'}

    def list_users(self):
        return [(u, info['admin']) for u, info in self.users.items()]

    def create_user(self, username: str, password: str, is_admin: bool) -> bool:
        if username in self.users:
            return False
        self.users[username] = {'password': password, 'admin': bool(is_admin)}
        self._save()
        return True

    def delete_user(self, username: str) -> bool:
        if username not in self.users:
            return False
        # Prevent deleting last admin
        if self.users[username]['admin']:
            admin_count = sum(1 for v in self.users.values() if v['admin'])
            if admin_count <= 1:
                return False
        del self.users[username]
        self._save()
        return True

    def _save(self):
        lines = []
        for u, info in self.users.items():
            lines.append(f"{u},{info['password']},{'1' if info['admin'] else '0'}")
        self.file.parent.mkdir(parents=True, exist_ok=True)
        self.file.write_text("\n".join(lines) + ("\n" if lines else ""), encoding='utf-8')


class AdminUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Admin - Gestión de Usuarios (Local)")
        self.geometry("520x420")

        self.gestor = GestorUsuariosLocal()

        top = ttk.Frame(self)
        top.pack(fill=tk.X, padx=8, pady=8)

        ttk.Label(top, text="Usuario:").grid(row=0, column=0, sticky=tk.W)
        self.user_var = tk.StringVar()
        ttk.Entry(top, textvariable=self.user_var, width=20).grid(row=0, column=1)

        ttk.Label(top, text="Clave:").grid(row=0, column=2, sticky=tk.W, padx=(8,0))
        self.pwd_var = tk.StringVar()
        ttk.Entry(top, textvariable=self.pwd_var, width=16, show='*').grid(row=0, column=3)

        self.admin_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(top, text="Admin", variable=self.admin_var).grid(row=0, column=4, padx=(8,0))

        ttk.Button(top, text="Crear", command=self.create_user).grid(row=0, column=5, padx=(8,0))

        # Lista de usuarios
        self.tree = ttk.Treeview(self, columns=("rol",), show='headings', selectmode='browse')
        self.tree.heading('rol', text='Rol (Admin)')
        self.tree.pack(fill=tk.BOTH, expand=True, padx=8, pady=8)

        btns = ttk.Frame(self)
        btns.pack(fill=tk.X, padx=8, pady=(0,8))
        ttk.Button(btns, text="Refrescar", command=self.refresh).pack(side=tk.LEFT)
        ttk.Button(btns, text="Eliminar seleccionado", command=self.delete_selected).pack(side=tk.LEFT, padx=(8,0))

        self.refresh()

    def refresh(self):
        for i in self.tree.get_children():
            self.tree.delete(i)
        for user, is_admin in self.gestor.list_users():
            self.tree.insert('', tk.END, iid=user, values=('Admin' if is_admin else 'Usuario',))

    def create_user(self):
        u = self.user_var.get().strip()
        p = self.pwd_var.get().strip()
        a = self.admin_var.get()
        if not u or not p:
            messagebox.showwarning("Datos incompletos", "Usuario y clave son requeridos")
            return
        ok = self.gestor.create_user(u, p, a)
        if not ok:
            messagebox.showerror("Error", "El usuario ya existe")
            return
        messagebox.showinfo("Creado", f"Usuario '{u}' creado")
        self.user_var.set("")
        self.pwd_var.set("")
        self.admin_var.set(False)
        self.refresh()

    def delete_selected(self):
        sel = self.tree.selection()
        if not sel:
            return
        user = sel[0]
        answer = messagebox.askyesno("Confirmar", f"Eliminar usuario '{user}'?")
        if not answer:
            return
        ok = self.gestor.delete_user(user)
        if not ok:
            messagebox.showerror("Error", "No se pudo eliminar (tal vez es el ultimo admin)")
            return
        messagebox.showinfo("Eliminado", f"Usuario '{user}' eliminado")
        self.refresh()


def run():
    app = AdminUI()
    app.mainloop()


if __name__ == '__main__':
    run()


def build_admin_frame(parent, repo_root: Path = None):
    """Construye y devuelve un `ttk.Frame` con la UI de administración.

    - `parent`: widget padre (por ejemplo `tk.Tk()` o `tk.Toplevel()`)
    - `repo_root`: ruta opcional para localizar `server/usuarios.db`
    """
    gestor = GestorUsuariosLocal(repo_root)

    frame = ttk.Frame(parent)

    top = ttk.Frame(frame)
    top.pack(fill=tk.X, padx=8, pady=8)

    ttk.Label(top, text="Usuario:").grid(row=0, column=0, sticky=tk.W)
    user_var = tk.StringVar()
    ttk.Entry(top, textvariable=user_var, width=20).grid(row=0, column=1)

    ttk.Label(top, text="Clave:").grid(row=0, column=2, sticky=tk.W, padx=(8,0))
    pwd_var = tk.StringVar()
    ttk.Entry(top, textvariable=pwd_var, width=16, show='*').grid(row=0, column=3)

    admin_var = tk.BooleanVar(value=False)
    ttk.Checkbutton(top, text="Admin", variable=admin_var).grid(row=0, column=4, padx=(8,0))

    def refresh():
        for i in tree.get_children():
            tree.delete(i)
        for user, is_admin in gestor.list_users():
            tree.insert('', tk.END, iid=user, values=('Admin' if is_admin else 'Usuario',))

    def create_user_cb():
        u = user_var.get().strip()
        p = pwd_var.get().strip()
        a = admin_var.get()
        if not u or not p:
            messagebox.showwarning("Datos incompletos", "Usuario y clave son requeridos")
            return
        ok = gestor.create_user(u, p, a)
        if not ok:
            messagebox.showerror("Error", "El usuario ya existe")
            return
        messagebox.showinfo("Creado", f"Usuario '{u}' creado")
        user_var.set("")
        pwd_var.set("")
        admin_var.set(False)
        refresh()

    def delete_selected_cb():
        sel = tree.selection()
        if not sel:
            return
        user = sel[0]
        answer = messagebox.askyesno("Confirmar", f"Eliminar usuario '{user}'?")
        if not answer:
            return
        ok = gestor.delete_user(user)
        if not ok:
            messagebox.showerror("Error", "No se pudo eliminar (tal vez es el ultimo admin)")
            return
        messagebox.showinfo("Eliminado", f"Usuario '{user}' eliminado")
        refresh()

    ttk.Button(top, text="Crear", command=create_user_cb).grid(row=0, column=5, padx=(8,0))

    tree = ttk.Treeview(frame, columns=("rol",), show='headings', selectmode='browse')
    tree.heading('rol', text='Rol (Admin)')
    tree.pack(fill=tk.BOTH, expand=True, padx=8, pady=8)

    btns = ttk.Frame(frame)
    btns.pack(fill=tk.X, padx=8, pady=(0,8))
    ttk.Button(btns, text="Refrescar", command=refresh).pack(side=tk.LEFT)
    ttk.Button(btns, text="Eliminar seleccionado", command=delete_selected_cb).pack(side=tk.LEFT, padx=(8,0))

    refresh()
    return frame


def create_admin_window(repo_root: Path = None):
    """Crea una ventana `Tk` con la UI de administración y la devuelve."""
    root = tk.Tk()
    root.title("Admin - Gestión de Usuarios (Local)")
    root.geometry("520x420")
    frame = build_admin_frame(root, repo_root=repo_root)
    frame.pack(fill=tk.BOTH, expand=True)
    return root
