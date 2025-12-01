# 3DS Save Sync

Synchroniser automatiquement les sauvegardes de jeux **Nintendo 3DS / NDS** (Checkpoint, JKSM, TWiLightMenu++) vers un **NAS distant**, via une application **homebrew 3DS**.

L’application fonctionne sur une 3DS moddée et s’appuie sur une connexion réseau déjà configurée (Wi-Fi, VPN en amont, etc.).

---

## 🎯 Objectif du projet

Fournir une solution simple pour :

- **Récupérer automatiquement** les sauvegardes de jeux NDS et 3DS depuis la carte SD.
- **Synchroniser ces sauvegardes vers un NAS** (HTTPS ou SFTP).
- **Télécharger une sauvegarde** depuis le NAS pour la restaurer sur la SD.
- Gérer une **configuration NAS modifiable directement depuis la 3DS**.

---

## 🔧 Fonctionnalités prévues

- Scan des saves :
  - Checkpoint → `sdmc:/3ds/Checkpoint/saves/...`
  - JKSM → `sdmc:/JKSV/Saves/...`
  - TWiLightMenu++ NDS → `sdmc:/_nds/TWiLightMenu/saves/...`
- **Upload** de dossiers complets de sauvegarde vers un NAS :
  - via HTTPS (API REST) ou SFTP simple.
- **Download + restore** :
  - récupérer une sauvegarde depuis le NAS et la restaurer dans le bon dossier sur la SD.
- **Compression** des sauvegardes à l’envoi :
  - ZIP ou TAR (décidé en implémentation).
- **Configuration éditable depuis l’interface 3DS** :
  - URL / IP du NAS, port, protocole (`https` / `sftp`), chemin distant, login, mot de passe.
- **Test de connexion** :
  - ping simple ou requête de test vers l’API / serveur SFTP.
- **Backend optionnel** :
  - mini API Python (Flask / FastAPI) de référence, ou dépôt SFTP brut.
  - possibilité de brancher ensuite un `rsync` côté NAS pour archivage / versionning.

---

## 🏗️ Architecture globale (3DS ↔ NAS)

Schéma ASCII simplifié :

```txt
+---------------------+            +-------------------------+
|  Nintendo 3DS       |            |  NAS / Serveur distant  |
|  (Homebrew)         |            |                         |
|                     |  HTTPS     |  API REST (upload,      |
|  - Scan saves       |<---------> |  list, download)        |
|  - Compression      |   ou       |                         |
|  - Config réseau    |   SFTP     |  ou                     |
|  - Sync upload/down |<---------> |  SFTP (arborescence     |
+---------------------+            |  simple de fichiers)    |
                                   +-------------------------+

               ^                                   |
               | (VPN géré en amont)              |
               +-----------------------------------+
```

📌 **Contrainte OpenVPN** : la 3DS ne gère pas OpenVPN directement. Le VPN (si nécessaire) doit être configuré sur le routeur ou via un smartphone / point d’accès. L’application 3DS ne fait que des requêtes HTTPS ou SFTP classiques vers une adresse déjà accessible.

---

## 📂 Structure proposée

```
3ds-save-sync/
├── README.md
├── 3ds-app/
│   ├── source/
│   ├── include/
│   ├── Makefile
│   └── config_example.json
├── server/
│   ├── app.py
│   ├── requirements.txt
│   └── storage/
└── docs/
    ├── protocol.md
    ├── config.md
    └── architecture.png
```

Consulte les fichiers dans `docs/` pour davantage de détails sur le protocole et le format de configuration.

---

## ⚙️ Compilation 3DS

Prérequis :

- devkitPro
- devkitARM
- libctru

Une fois l’environnement installé et les variables `DEVKITPRO` / `DEVKITARM` configurées :

```sh
cd 3ds-app
make
```

Un fichier `3ds-save-sync.3dsx` (ou similaire) sera généré et pourra être copié sur la carte SD pour Homebrew Launcher.

---

## 🚀 Roadmap

- **V0 – Compression locale** : scan des répertoires, compression ZIP/TAR locale, interface simple.
- **V1 – Sync LAN** : upload / download HTTPS ou SFTP, test de connexion, gestion d’erreurs de base.
- **V2 – Sync via VPN externe** : documentation et tests avec VPN configuré sur routeur / smartphone.
- **V3 – Restauration avancée** : historique de versions, sélection par jeu, UI améliorée.

---

## 🧩 Licence

À définir (MIT, GPL, etc.).
