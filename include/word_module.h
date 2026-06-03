#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// ============================================================
//  MODUL KATA - TYPING DEFENCE
//  Tema   : Space (easy) | Teknologi (medium) | Alam (hard)
//  Total  : 150 kata (50 per kelompok)
// ============================================================

// ------------------------------------------------------------
//  EASY  –  Tema: Space
// ------------------------------------------------------------
const std::vector<std::string> easy = {
    "star",        "moon",        "sun",         "mars",        "orbit",
    "comet",       "alien",       "probe",       "space",       "light",
    "dust",        "ring",        "rock",        "void",        "plasma",
    "nova",        "flux",        "core",        "axis",        "tide",
    "glow",        "haze",        "beam",        "wave",        "zone",
    "mass",        "spin",        "pull",        "drift",       "burst",
    "cloud",       "crater",      "flare",       "meteor",      "nebula",
    "quasar",      "pulsar",      "aurora",      "corona",      "photon",
    "zenith",      "eclipse",     "solstice",    "equinox",     "horizon",
    "cosmos",      "galaxy",      "saturn",      "jupiter",     "venus"
};

const std::unordered_map<std::string, std::string> easy_definitions = {
    {"star",      "Benda langit yang memancarkan cahaya dan panas dari reaksi nuklir di intinya"},
    {"moon",      "Satelit alami yang mengorbit sebuah planet"},
    {"sun",       "Bintang pusat tata surya kita yang menjadi sumber energi utama Bumi"},
    {"mars",      "Planet keempat dari Matahari yang dikenal sebagai planet merah"},
    {"orbit",     "Lintasan melengkung yang dilalui benda langit saat mengelilingi benda lain"},
    {"comet",     "Benda kecil tata surya yang mengeluarkan ekor gas saat mendekati Matahari"},
    {"alien",     "Makhluk atau organisme yang berasal dari luar Bumi"},
    {"probe",     "Wahana antariksa tak berawak yang dikirim untuk menjelajahi benda langit"},
    {"space",     "Ruang hampa yang luas di luar atmosfer Bumi"},
    {"light",     "Gelombang elektromagnetik yang dapat ditangkap oleh mata manusia"},
    {"dust",      "Partikel padat berukuran sangat kecil yang bertebaran di antariksa"},
    {"ring",      "Piringan material yang mengelilingi beberapa planet seperti Saturnus"},
    {"rock",      "Material padat berbentuk batu yang membentuk benda-benda langit padat"},
    {"void",      "Ruang kosong tanpa materi yang sangat luas di alam semesta"},
    {"plasma",    "Wujud materi keempat berupa gas terionisasi bermuatan listrik"},
    {"nova",      "Ledakan terang mendadak pada bintang yang meningkatkan kecerahannya"},
    {"flux",      "Perubahan atau aliran energi atau partikel secara terus-menerus"},
    {"core",      "Bagian paling dalam dari sebuah bintang atau planet"},
    {"axis",      "Garis khayal tempat sebuah benda langit berotasi"},
    {"tide",      "Naik turunnya permukaan air laut akibat gravitasi Bulan dan Matahari"},
    {"glow",      "Cahaya redup yang dipancarkan oleh gas atau benda langit tertentu"},
    {"haze",      "Kabut tipis di atmosfer planet akibat partikel kecil di udara"},
    {"beam",      "Sinar terarah yang dipancarkan dari sumber cahaya atau energi"},
    {"wave",      "Getaran yang merambat melalui ruang membawa energi"},
    {"zone",      "Wilayah tertentu di antariksa atau atmosfer dengan karakteristik khas"},
    {"mass",      "Jumlah materi yang terkandung dalam sebuah benda langit"},
    {"spin",      "Gerakan rotasi suatu benda langit pada porosnya sendiri"},
    {"pull",      "Gaya tarik gravitasi yang menarik benda ke arah pusat massa"},
    {"drift",     "Pergerakan perlahan benda di antariksa tanpa dorongan aktif"},
    {"burst",     "Ledakan energi singkat namun intens dari sebuah benda kosmik"},
    {"cloud",     "Kumpulan gas dan debu di antariksa yang membentuk nebula"},
    {"crater",    "Cekungan berbentuk mangkuk di permukaan benda langit akibat tumbukan"},
    {"flare",     "Pancaran tiba-tiba energi besar dari permukaan Matahari"},
    {"meteor",    "Batu angkasa yang terbakar saat memasuki atmosfer Bumi"},
    {"nebula",    "Awan gas dan debu di antariksa tempat bintang baru terbentuk"},
    {"quasar",    "Inti galaksi aktif yang sangat terang dan jauh di alam semesta"},
    {"pulsar",    "Bintang neutron yang memancarkan sinyal radio secara teratur"},
    {"aurora",    "Fenomena cahaya indah di langit kutub akibat interaksi partikel surya"},
    {"corona",    "Lapisan terluar atmosfer Matahari yang sangat panas"},
    {"photon",    "Partikel dasar pembawa energi cahaya yang tidak bermassa"},
    {"zenith",    "Titik tertinggi di langit tepat di atas pengamat"},
    {"eclipse",   "Peristiwa satu benda langit menutup benda langit lainnya"},
    {"solstice",  "Titik dalam setahun ketika Matahari berada paling jauh dari ekuator"},
    {"equinox",   "Saat ketika panjang siang dan malam hampir sama di seluruh Bumi"},
    {"horizon",   "Garis khayal yang memisahkan langit dan permukaan Bumi"},
    {"cosmos",    "Keseluruhan alam semesta beserta semua isinya yang teratur"},
    {"galaxy",    "Sistem besar berisi miliaran bintang, gas, dan debu yang terikat gravitasi"},
    {"saturn",    "Planet keenam dari Matahari yang terkenal dengan cincinnya yang indah"},
    {"jupiter",   "Planet terbesar di tata surya yang merupakan raksasa gas"},
    {"venus",     "Planet kedua dari Matahari dan yang paling panas di tata surya"}
};


// ------------------------------------------------------------
//  MEDIUM  –  Tema: Teknologi
// ------------------------------------------------------------
const std::vector<std::string> medium = {
    "pixel",       "cache",       "query",       "server",      "kernel",
    "buffer",      "router",      "socket",      "thread",      "binary",
    "syntax",      "output",      "input",       "module",      "script",
    "debug",       "branch",      "deploy",      "render",      "compile",
    "latency",     "firewall",    "protocol",    "database",    "function",
    "variable",    "instance",    "callback",    "iterator",    "operator",
    "network",     "gateway",     "payload",     "endpoint",    "pipeline",
    "webhook",     "runtime",     "package",     "library",     "interface",
    "algorithm",   "recursion",   "pointer",     "hardware",    "software",
    "firmware",    "bandwidth",   "encryption",  "framework",   "container"
};

const std::unordered_map<std::string, std::string> medium_definitions = {
    {"pixel",      "Unit terkecil dari gambar digital yang ditampilkan di layar"},
    {"cache",      "Penyimpanan sementara data agar akses berikutnya lebih cepat"},
    {"query",      "Permintaan atau pertanyaan yang dikirimkan ke basis data"},
    {"server",     "Komputer atau program yang menyediakan layanan untuk komputer lain"},
    {"kernel",     "Inti dari sistem operasi yang mengelola sumber daya perangkat keras"},
    {"buffer",     "Area memori sementara untuk menyimpan data saat dipindahkan"},
    {"router",     "Perangkat jaringan yang meneruskan paket data antar jaringan"},
    {"socket",     "Antarmuka komunikasi dua arah antar proses melalui jaringan"},
    {"thread",     "Unit eksekusi terkecil yang berjalan dalam sebuah proses"},
    {"binary",     "Sistem bilangan berbasis dua yang hanya menggunakan angka 0 dan 1"},
    {"syntax",     "Aturan tata bahasa yang mengatur penulisan kode program"},
    {"output",     "Data atau informasi yang dihasilkan oleh sebuah program atau sistem"},
    {"input",      "Data atau informasi yang dimasukkan ke dalam sistem atau program"},
    {"module",     "Komponen perangkat lunak yang dapat digunakan kembali secara mandiri"},
    {"script",     "Kumpulan instruksi yang dieksekusi langsung tanpa kompilasi"},
    {"debug",      "Proses menemukan dan memperbaiki kesalahan dalam kode program"},
    {"branch",     "Cabang dalam sistem kontrol versi yang merepresentasikan alur pengembangan"},
    {"deploy",     "Proses menerbitkan dan menjalankan aplikasi di lingkungan produksi"},
    {"render",     "Proses mengubah kode atau data menjadi tampilan visual"},
    {"compile",    "Proses menerjemahkan kode sumber menjadi kode yang dapat dijalankan"},
    {"latency",    "Waktu tunda antara permintaan dikirim dan respons diterima"},
    {"firewall",   "Sistem keamanan yang memfilter lalu lintas jaringan masuk dan keluar"},
    {"protocol",   "Aturan komunikasi standar antar perangkat dalam jaringan"},
    {"database",   "Kumpulan data terstruktur yang disimpan dan dikelola secara sistematis"},
    {"function",   "Blok kode yang dapat dipanggil berulang kali untuk tugas tertentu"},
    {"variable",   "Tempat penyimpanan data dalam memori yang nilainya dapat berubah"},
    {"instance",   "Objek konkret yang dibuat dari sebuah kelas dalam pemrograman berorientasi objek"},
    {"callback",   "Fungsi yang diteruskan sebagai argumen dan dipanggil setelah proses selesai"},
    {"iterator",   "Objek yang memungkinkan penelusuran elemen koleksi satu per satu"},
    {"operator",   "Simbol atau kata kunci yang melakukan operasi pada satu atau lebih nilai"},
    {"network",    "Kumpulan perangkat yang saling terhubung untuk berbagi data"},
    {"gateway",    "Titik masuk jaringan yang menghubungkan dua jaringan berbeda"},
    {"payload",    "Data utama yang dibawa dalam sebuah paket transmisi"},
    {"endpoint",   "URL atau titik akhir tempat layanan API menerima permintaan"},
    {"pipeline",   "Rangkaian proses otomatis yang dijalankan secara berurutan"},
    {"webhook",    "Mekanisme HTTP yang memicu notifikasi otomatis saat suatu peristiwa terjadi"},
    {"runtime",    "Lingkungan tempat program dieksekusi pada saat berjalan"},
    {"package",    "Kumpulan modul atau library yang dikemas untuk didistribusikan"},
    {"library",    "Kumpulan kode siap pakai yang dapat digunakan dalam program lain"},
    {"interface",  "Kontrak yang mendefinisikan metode tanpa mengimplementasikannya"},
    {"algorithm",  "Langkah-langkah logis dan terstruktur untuk menyelesaikan suatu masalah"},
    {"recursion",  "Teknik pemrograman di mana fungsi memanggil dirinya sendiri"},
    {"pointer",    "Variabel yang menyimpan alamat memori dari variabel lain"},
    {"hardware",   "Komponen fisik dari sistem komputer yang dapat disentuh"},
    {"software",   "Program dan data yang berjalan di atas perangkat keras komputer"},
    {"firmware",   "Perangkat lunak permanen yang tertanam langsung di perangkat keras"},
    {"bandwidth",  "Kapasitas maksimum data yang dapat ditransmisikan dalam jaringan per satuan waktu"},
    {"encryption", "Proses mengubah data menjadi bentuk tersandi agar tidak terbaca pihak lain"},
    {"framework",  "Kerangka kerja perangkat lunak yang menyediakan struktur dasar pengembangan aplikasi"},
    {"container",  "Lingkungan terisolasi untuk menjalankan aplikasi dengan semua dependensinya"}
};


// ------------------------------------------------------------
//  HARD  –  Tema: Alam
// ------------------------------------------------------------
const std::vector<std::string> hard = {
    "photosynthesis",  "biodiversity",    "ecosystem",       "atmosphere",      "geothermal",
    "evaporation",     "continental",     "stratosphere",    "troposphere",     "hydrosphere",
    "lithosphere",     "biosphere",       "permafrost",      "sedimentary",     "igneous",
    "metamorphic",     "topography",      "watershed",       "mangrove",        "savanna",
    "tundra",          "boreal",          "deciduous",       "coniferous",      "estuary",
    "fjord",           "plateau",         "archipelago",     "peninsula",       "isthmus",
    "lagoon",          "delta",           "glacier",         "aquifer",         "tsunami",
    "typhoon",         "monsoon",         "cyclone",         "tornado",         "drought",
    "erosion",         "sedimentation",   "decomposition",   "pollination",     "migration",
    "hibernation",     "symbiosis",       "predation",       "commensalism",    "mutualism"
};

const std::unordered_map<std::string, std::string> hard_definitions = {
    {"photosynthesis",  "Proses tumbuhan mengubah cahaya matahari, air, dan CO2 menjadi energi dan oksigen"},
    {"biodiversity",    "Keragaman seluruh makhluk hidup di suatu wilayah atau ekosistem"},
    {"ecosystem",       "Sistem yang terbentuk dari interaksi makhluk hidup dengan lingkungan fisiknya"},
    {"atmosphere",      "Lapisan gas yang menyelimuti Bumi dan melindunginya dari radiasi luar angkasa"},
    {"geothermal",      "Energi panas yang berasal dari dalam inti Bumi"},
    {"evaporation",     "Proses perubahan wujud air dari cair menjadi uap akibat panas"},
    {"continental",     "Berkaitan dengan daratan besar atau lempeng benua yang luas"},
    {"stratosphere",    "Lapisan atmosfer kedua yang berada di atas troposfer, tempat ozon berada"},
    {"troposphere",     "Lapisan atmosfer paling bawah tempat berlangsungnya cuaca dan iklim"},
    {"hydrosphere",     "Semua air yang ada di Bumi, mencakup lautan, danau, sungai, dan es"},
    {"lithosphere",     "Lapisan padat paling luar Bumi yang terdiri dari kerak dan mantel atas"},
    {"biosphere",       "Seluruh zona di Bumi tempat kehidupan dapat berlangsung"},
    {"permafrost",      "Lapisan tanah yang membeku secara permanen di daerah kutub dan subarktik"},
    {"sedimentary",     "Jenis batuan yang terbentuk dari endapan material yang mengeras seiring waktu"},
    {"igneous",         "Batuan yang terbentuk dari pembekuan magma atau lava gunung berapi"},
    {"metamorphic",     "Batuan yang berubah bentuk akibat tekanan dan panas tinggi di dalam bumi"},
    {"topography",      "Studi atau peta tentang bentuk dan fitur permukaan bumi suatu wilayah"},
    {"watershed",       "Wilayah tangkapan air yang mengalirkan semua airnya ke satu sungai atau danau"},
    {"mangrove",        "Hutan bakau di wilayah pesisir tropis yang tahan terhadap air asin"},
    {"savanna",         "Padang rumput tropis dengan pohon-pohon yang tersebar dan musim kering panjang"},
    {"tundra",          "Bioma tanpa pohon di daerah kutub dengan tanah beku dan vegetasi rendah"},
    {"boreal",          "Berkaitan dengan hutan konifer dingin di wilayah subarktik belahan utara"},
    {"deciduous",       "Tumbuhan yang menggugurkan daunnya secara musiman, terutama di musim dingin"},
    {"coniferous",      "Tumbuhan berdaun jarum yang menghasilkan biji dalam bentuk runjung"},
    {"estuary",         "Muara sungai di mana air tawar dan air laut bertemu dan bercampur"},
    {"fjord",           "Teluk panjang dan sempit yang terbentuk dari lembah glasial yang terendam air laut"},
    {"plateau",         "Dataran tinggi yang relatif datar dan dikelilingi oleh tebing curam"},
    {"archipelago",     "Gugusan pulau-pulau yang tersebar di lautan dalam satu kawasan"},
    {"peninsula",       "Daratan yang menjorok ke laut dan dikelilingi air di tiga sisinya"},
    {"isthmus",         "Tanah sempit yang menghubungkan dua daratan besar di antara dua lautan"},
    {"lagoon",          "Perairan dangkal yang terpisah dari laut oleh beting pasir atau terumbu karang"},
    {"delta",           "Endapan lumpur berbentuk kipas di muara sungai yang bertemu lautan"},
    {"glacier",         "Masa es besar yang bergerak perlahan di atas daratan akibat akumulasi salju"},
    {"aquifer",         "Lapisan batuan atau tanah yang mengandung dan menyimpan air tanah"},
    {"tsunami",         "Gelombang laut raksasa yang dipicu oleh gempa bumi atau letusan bawah laut"},
    {"typhoon",         "Badai tropis kencang yang terbentuk di wilayah Pasifik barat"},
    {"monsoon",         "Angin musiman yang membawa perubahan curah hujan signifikan ke suatu wilayah"},
    {"cyclone",         "Sistem cuaca berputar dengan tekanan rendah di pusatnya"},
    {"tornado",         "Kolom udara berputar dengan kecepatan sangat tinggi yang menyentuh tanah"},
    {"drought",         "Kekurangan air berkepanjangan akibat curah hujan jauh di bawah normal"},
    {"erosion",         "Pengikisan permukaan tanah atau batuan oleh air, angin, atau es"},
    {"sedimentation",   "Proses pengendapan partikel padat yang terbawa oleh air atau angin"},
    {"decomposition",   "Proses penguraian bahan organik oleh mikroorganisme menjadi zat sederhana"},
    {"pollination",     "Proses pemindahan serbuk sari dari kepala sari ke putik bunga"},
    {"migration",       "Perpindahan hewan secara musiman dari satu wilayah ke wilayah lain"},
    {"hibernation",     "Kondisi istirahat panjang hewan selama musim dingin untuk menghemat energi"},
    {"symbiosis",       "Hubungan erat antara dua spesies berbeda yang hidup bersama"},
    {"predation",       "Interaksi di mana satu organisme memangsa organisme lain sebagai makanan"},
    {"commensalism",    "Hubungan di mana satu pihak diuntungkan dan pihak lain tidak terpengaruh"},
    {"mutualism",       "Hubungan simbiosis di mana kedua pihak saling mendapat keuntungan"}
};
