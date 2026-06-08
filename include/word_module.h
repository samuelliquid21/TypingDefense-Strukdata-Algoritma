#pragma once
#include <string>
#include <vector>
#include <unordered_map>

// ============================================================
//  MODUL KATA 2 - TYPING DEFENCE
//  Pengelompokan berdasarkan jumlah huruf:
//    Easy   :  1 -  5 huruf  (50 kata)
//    Medium :  6 - 10 huruf  (50 kata)
//    Hard   : 11+    huruf   (50 kata)
//  Tema bebas (campuran: sains, alam, teknologi, sehari-hari)
// ============================================================


// ------------------------------------------------------------
//  EASY  -  1 sampai 5 huruf  (50 kata)
//  Pool kata mudah: kosakata pendek, umum, mudah diketik
// ------------------------------------------------------------
const std::vector<std::string> easy = {
    // 1 huruf
    "a",
    // 2 huruf
    "ox", "ax",
    // 3 huruf
    "ant", "bee", "cat", "dog", "egg", "fan", "fog",
    "gem", "hat", "ice", "jar", "key", "log", "map",
    "net", "oak", "pen", "rod", "sky", "tea", "urn",
    // 4 huruf
    "atom", "bear", "cake", "dark", "echo", "frog",
    "gust", "hawk", "iris", "jade", "kite", "lava",
    "mist", "nest", "oval", "pine", "quiz", "rain",
    "sage", "toad", "ulna", "vine", "wasp", "yolk",
    // 5 huruf
    "blend", "coral"
};

// Definisi bahasa Indonesia untuk setiap kata Easy
const std::unordered_map<std::string, std::string> easy_definitions = {
    {"a",     "Artikel tak tentu dalam bahasa Inggris"},
    {"ox",    "Sapi jantan besar yang sering digunakan sebagai hewan penarik beban"},
    {"ax",    "Alat pemotong kayu dengan mata besi tajam dan bertangkai panjang"},
    {"ant",   "Serangga kecil yang hidup berkoloni dan dikenal sangat kuat"},
    {"bee",   "Serangga bersayap penghasil madu yang berperan penting dalam penyerbukan"},
    {"cat",   "Hewan peliharaan berkaki empat yang dikenal gesit dan mandiri"},
    {"dog",   "Hewan peliharaan setia yang sering dijadikan sahabat manusia"},
    {"egg",   "Sel reproduksi berbentuk oval yang dihasilkan oleh hewan betina"},
    {"fan",   "Alat yang menghasilkan aliran udara untuk menyejukkan ruangan"},
    {"fog",   "Awan tipis yang berada dekat permukaan tanah sehingga mengurangi jarak pandang"},
    {"gem",   "Batu mulia atau berharga yang digunakan sebagai perhiasan"},
    {"hat",   "Penutup kepala yang digunakan sebagai pelindung atau aksesori"},
    {"ice",   "Air yang membeku menjadi padat pada suhu di bawah nol derajat Celsius"},
    {"jar",   "Wadah kaca atau plastik bermulut lebar untuk menyimpan bahan"},
    {"key",   "Alat kecil dari logam untuk membuka atau mengunci pintu dan gembok"},
    {"log",   "Batang pohon yang sudah ditebang dan dipotong"},
    {"map",   "Gambar atau representasi permukaan bumi dalam skala yang diperkecil"},
    {"net",   "Jaring yang terbuat dari benang atau tali yang dianyam bersilang"},
    {"oak",   "Pohon besar penghasil kayu keras yang banyak ditemukan di daerah beriklim sedang"},
    {"pen",   "Alat tulis yang menggunakan tinta untuk menulis atau menggambar"},
    {"rod",   "Batang panjang dan lurus yang terbuat dari logam atau kayu"},
    {"sky",   "Ruang atau cakrawala yang terlihat di atas permukaan Bumi"},
    {"tea",   "Minuman yang dibuat dari seduhan daun tanaman Camellia sinensis"},
    {"urn",   "Wadah berbentuk vas besar, sering digunakan untuk menyimpan abu jenazah"},
    {"atom",  "Unit terkecil dari unsur kimia yang masih mempertahankan sifat unsur tersebut"},
    {"bear",  "Mamalia besar berbulu yang hibernasi di musim dingin"},
    {"cake",  "Kue manis yang dipanggang, sering disajikan saat perayaan"},
    {"dark",  "Kondisi tidak adanya atau sangat sedikitnya cahaya"},
    {"echo",  "Pantulan bunyi yang terdengar kembali setelah menabrak permukaan keras"},
    {"frog",  "Amfibi yang hidup di air dan darat serta melompat menggunakan kaki belakangnya"},
    {"gust",  "Hembusan angin kencang yang tiba-tiba"},
    {"hawk",  "Burung pemangsa berpenglihatan tajam dari keluarga Accipitridae"},
    {"iris",  "Bagian berwarna pada mata yang mengatur besar kecilnya pupil"},
    {"jade",  "Batu permata hijau yang bernilai tinggi dalam budaya Asia"},
    {"kite",  "Mainan dari kertas atau kain yang diterbangkan menggunakan tali di udara"},
    {"lava",  "Magma panas yang keluar dari gunung berapi dan mengalir di permukaan"},
    {"mist",  "Kabut tipis berupa tetesan air kecil yang melayang di udara"},
    {"nest",  "Tempat yang dibangun hewan untuk bertelur dan membesarkan anaknya"},
    {"oval",  "Bentuk geometris menyerupai elips atau telur yang memanjang"},
    {"pine",  "Pohon konifer hijau sepanjang tahun yang menghasilkan biji dalam runjung"},
    {"quiz",  "Tes atau pertanyaan singkat untuk mengukur pengetahuan seseorang"},
    {"rain",  "Air yang jatuh dari awan ke permukaan bumi dalam bentuk tetes"},
    {"sage",  "Tanaman herbal aromatik yang digunakan dalam memasak dan pengobatan"},
    {"toad",  "Amfibi mirip katak dengan kulit berbenjol yang lebih banyak di darat"},
    {"ulna",  "Salah satu tulang lengan bawah manusia yang terletak di sisi kelingking"},
    {"vine",  "Tanaman merambat yang tumbuh dengan melilit atau menempel pada benda lain"},
    {"wasp",  "Serangga bersengat yang memiliki tubuh ramping dan berwarna kuning hitam"},
    {"yolk",  "Bagian kuning dari telur yang kaya nutrisi dan lemak"},
    {"blend", "Mencampurkan dua atau lebih bahan menjadi satu kesatuan yang seragam"},
    {"coral", "Organisme laut kecil yang membentuk koloni dan membangun terumbu karang"}
};


// ------------------------------------------------------------
//  MEDIUM  -  6 sampai 10 huruf  (50 kata)
//  Pool kata sedang: panjang kata sedang, tantangan mengetik menengah
// ------------------------------------------------------------
const std::vector<std::string> medium = {
    // 6 huruf
    "arctic", "bottle", "cactus", "dagger", "embryo",
    "fossil", "goblin", "harbor", "ignite", "jungle",
    "kelvin", "locket", "magnet", "noodle", "oyster",
    "pollen", "quartz", "rabbit", "spider", "tundra",
    // 7 huruf
    "abyssal", "beneath", "capsule", "diamond", "eclipse",
    "forward", "granite", "harvest", "insight", "jupiter",
    // 8 huruf
    "altitude", "blizzard", "chemical", "download", "electron",
    "fracture", "gradient", "hologram", "infrared", "jealousy",
    // 9 huruf
    "amplified", "bacterium", "carnivore", "darkening", "enumerate",
    "formation", "germinate", "herbivore", "implement", "jellyfish"
};

// Definisi bahasa Indonesia untuk setiap kata Medium
const std::unordered_map<std::string, std::string> medium_definitions = {
    {"arctic",     "Wilayah kutub utara Bumi yang tertutup es dan sangat dingin"},
    {"bottle",     "Wadah silindris bermulut sempit untuk menyimpan cairan"},
    {"cactus",     "Tanaman berduri yang mampu bertahan di lingkungan gurun yang kering"},
    {"dagger",     "Senjata tajam berupa pisau pendek bermata dua untuk menikam"},
    {"embryo",     "Organisme dalam tahap paling awal perkembangannya setelah pembuahan"},
    {"fossil",     "Sisa atau jejak makhluk hidup purba yang terawetkan dalam batuan"},
    {"goblin",     "Makhluk kecil jahat dalam mitologi yang dikenal nakal dan licik"},
    {"harbor",     "Kawasan perairan terlindung tempat kapal berlabuh dengan aman"},
    {"ignite",     "Membakar atau menyalakan sesuatu sehingga menghasilkan api"},
    {"jungle",     "Hutan lebat tropis dengan vegetasi yang rapat dan keanekaragaman tinggi"},
    {"kelvin",     "Satuan suhu dalam sistem SI yang dimulai dari nol absolut"},
    {"locket",     "Liontin kecil yang dapat dibuka dan berisi foto atau kenang-kenangan"},
    {"magnet",     "Benda yang memiliki gaya tarik terhadap material besi atau baja"},
    {"noodle",     "Makanan berbahan tepung yang dicetak panjang tipis dan direbus"},
    {"oyster",     "Moluska laut bercangkang yang dapat menghasilkan mutiara"},
    {"pollen",     "Serbuk halus yang dihasilkan benang sari bunga untuk penyerbukan"},
    {"quartz",     "Mineral silika yang umum ditemukan di kerak Bumi dalam berbagai warna"},
    {"rabbit",     "Mamalia kecil berbulu dengan telinga panjang yang bergerak melompat"},
    {"spider",     "Artropoda berdelapan kaki yang menghasilkan jaring sutra untuk menjebak mangsa"},
    {"tundra",     "Bioma dingin tanpa pohon di daerah kutub dengan tanah beku permanen"},
    {"abyssal",    "Berkaitan dengan kedalaman lautan yang sangat dalam dan gelap"},
    {"beneath",    "Di bawah atau di sisi yang lebih rendah dari sesuatu"},
    {"capsule",    "Wadah kecil tertutup rapat untuk obat atau benda berharga"},
    {"diamond",    "Bentuk karbon paling keras yang digunakan sebagai perhiasan dan alat industri"},
    {"eclipse",    "Peristiwa satu benda langit menghalangi cahaya ke benda langit lainnya"},
    {"forward",    "Bergerak atau mengarah ke depan menuju tujuan"},
    {"granite",    "Batuan beku keras bertekstur kasar yang sering digunakan dalam konstruksi"},
    {"harvest",    "Proses memanen atau mengumpulkan hasil pertanian yang sudah matang"},
    {"insight",    "Pemahaman mendalam tentang sesuatu yang diperoleh melalui pengamatan"},
    {"jupiter",    "Planet terbesar di tata surya yang merupakan raksasa gas"},
    {"altitude",   "Ketinggian suatu titik di atas permukaan laut atau permukaan Bumi"},
    {"blizzard",   "Badai salju lebat disertai angin kencang dengan jarak pandang sangat rendah"},
    {"chemical",   "Berkaitan dengan zat atau proses yang melibatkan reaksi kimia"},
    {"download",   "Proses mengunduh atau menyalin data dari internet ke perangkat lokal"},
    {"electron",   "Partikel subatomik bermuatan negatif yang mengelilingi inti atom"},
    {"fracture",   "Retakan atau patahan pada benda padat akibat gaya atau tekanan"},
    {"gradient",   "Tingkat perubahan nilai suatu besaran terhadap jarak atau posisi"},
    {"hologram",   "Gambar tiga dimensi yang direkam menggunakan cahaya laser"},
    {"infrared",   "Radiasi elektromagnetik dengan panjang gelombang lebih panjang dari cahaya merah"},
    {"jealousy",   "Perasaan tidak senang atau cemas akibat kelebihan atau keberhasilan orang lain"},
    {"amplified",  "Diperbesar atau diperkuat, terutama merujuk pada sinyal atau suara"},
    {"bacterium",  "Organisme bersel tunggal prokariotik mikroskopis yang tersebar di mana-mana"},
    {"carnivore",  "Hewan yang memperoleh energi dari memakan daging hewan lain"},
    {"darkening",  "Proses menjadi lebih gelap atau berkurangnya intensitas cahaya"},
    {"enumerate",  "Menyebutkan atau mendaftar sesuatu satu per satu secara berurutan"},
    {"formation",  "Proses terbentuknya sesuatu atau susunan yang terbentuk dari proses tersebut"},
    {"germinate",  "Proses biji mulai tumbuh dan berkembang menjadi tanaman baru"},
    {"herbivore",  "Hewan yang hanya memakan tumbuhan sebagai sumber energinya"},
    {"implement",  "Menerapkan atau melaksanakan suatu rencana atau sistem secara nyata"},
    {"jellyfish",  "Hewan laut bertubuh lunak berbentuk payung dan tidak bertulang belakang"}
};


// ------------------------------------------------------------
//  HARD  -  11 huruf ke atas  (50 kata)
//  Pool kata sulit: kata panjang, tantangan mengetik tingkat tinggi
// ------------------------------------------------------------
const std::vector<std::string> hard = {
    // 11 huruf
    "archipelago",          "biodiversity",         "computation",
    "deforestation",        "electrolysis",         "fluorescence",
    "geochemistry",         "hibernation",          "illustration",
    "jurisdiction",
    // 12 huruf
    "acceleration",         "biomechanics",         "conservation",
    "disintegrate",         "echolocation",         "fermentation",
    "geomorphology",        "hallucination",        "inflammation",
    "juxtaposition",
    // 13 huruf
    "amplification",        "bioluminescent",       "circumference",
    "decomposition",        "electrostatic",        "fossilization",
    "gravitational",        "hydroelectric",        "immunotherapy",
    "justification",
    // 14 huruf
    "bioengineering",       "chemosynthesis",       "decentralized",
    "electrochemist",       "fossiliferous",        "geochronology",
    "hematological",        "instrumentation",      "jurisprudence",
    "keratinization",
    // 15+ huruf
    "biogeochemical",       "chemiluminescence",    "deoxyribonucleic",
    "electromagnetism",     "photosynthetically",   "superconductivity",
    "thermodynamically",    "unconstitutional",     "underrepresented",
    "biostratigraphically"
};

// Definisi bahasa Indonesia untuk setiap kata Hard
const std::unordered_map<std::string, std::string> hard_definitions = {
    {"archipelago",           "Gugusan pulau-pulau yang tersebar di lautan dalam satu kawasan"},
    {"biodiversity",          "Keanekaragaman seluruh makhluk hidup yang ada di suatu ekosistem atau wilayah"},
    {"computation",           "Proses melakukan perhitungan atau pengolahan data secara sistematis"},
    {"deforestation",         "Penebangan atau perusakan hutan secara besar-besaran yang mengancam ekosistem"},
    {"electrolysis",          "Proses kimia penguraian senyawa menggunakan arus listrik"},
    {"fluorescence",          "Emisi cahaya oleh zat yang menyerap radiasi lalu memancarkannya kembali"},
    {"geochemistry",          "Ilmu yang mempelajari komposisi kimia Bumi dan proses kimia di dalamnya"},
    {"hibernation",           "Kondisi istirahat panjang hewan selama musim dingin untuk menghemat energi"},
    {"illustration",          "Gambar atau representasi visual yang digunakan untuk memperjelas suatu konsep"},
    {"jurisdiction",          "Wilayah atau kewenangan hukum yang dimiliki oleh suatu lembaga atau negara"},
    {"acceleration",          "Perubahan kecepatan suatu benda per satuan waktu"},
    {"biomechanics",          "Ilmu yang mempelajari prinsip-prinsip mekanika pada sistem biologis"},
    {"conservation",          "Upaya perlindungan dan pelestarian sumber daya alam dan lingkungan hidup"},
    {"disintegrate",          "Proses hancur atau terurai menjadi bagian-bagian kecil yang terpisah"},
    {"echolocation",          "Kemampuan hewan seperti kelelawar menggunakan pantulan suara untuk navigasi"},
    {"fermentation",          "Proses penguraian senyawa organik oleh mikroorganisme tanpa oksigen"},
    {"geomorphology",         "Ilmu yang mempelajari bentuk permukaan Bumi dan proses pembentukannya"},
    {"hallucination",         "Persepsi sensorik yang terjadi tanpa stimulus nyata dari luar"},
    {"inflammation",          "Respons biologis jaringan tubuh terhadap infeksi atau cedera"},
    {"juxtaposition",         "Penempatan dua hal berbeda secara berdampingan untuk perbandingan kontras"},
    {"amplification",         "Proses memperkuat sinyal, suara, atau efek sehingga menjadi lebih besar"},
    {"bioluminescent",        "Kemampuan organisme hidup menghasilkan dan memancarkan cahaya secara biologis"},
    {"circumference",         "Panjang keliling lingkaran atau batas luar suatu benda bulat"},
    {"decomposition",         "Proses penguraian bahan organik oleh mikroorganisme menjadi zat lebih sederhana"},
    {"electrostatic",         "Berkaitan dengan muatan listrik yang diam dan gaya yang ditimbulkannya"},
    {"fossilization",         "Proses terbentuknya fosil dari sisa organisme yang terawetkan dalam batuan"},
    {"gravitational",         "Berkaitan dengan gaya tarik yang bekerja antara dua massa"},
    {"hydroelectric",         "Berkaitan dengan pembangkitan listrik menggunakan energi aliran air"},
    {"immunotherapy",         "Pengobatan penyakit dengan cara memanipulasi sistem kekebalan tubuh"},
    {"justification",         "Alasan atau pembenaran logis yang mendasari suatu tindakan atau keputusan"},
    {"bioengineering",        "Penerapan prinsip rekayasa dalam sistem biologis dan medis"},
    {"chemosynthesis",        "Proses organisme menghasilkan energi dari reaksi kimia tanpa cahaya matahari"},
    {"decentralized",         "Sistem yang tidak terpusat dan menyebarkan kendali ke banyak titik"},
    {"electrochemist",        "Ilmuwan yang mempelajari hubungan antara reaksi kimia dan energi listrik"},
    {"fossiliferous",         "Mengandung atau kaya akan fosil dari organisme purba"},
    {"geochronology",         "Ilmu penentuan usia batuan dan peristiwa geologis menggunakan metode ilmiah"},
    {"hematological",         "Berkaitan dengan ilmu atau kondisi yang menyangkut darah dan komponennya"},
    {"instrumentation",       "Perangkat atau sistem alat ukur yang digunakan dalam eksperimen atau industri"},
    {"jurisprudence",         "Ilmu atau teori yang membahas filosofi dan prinsip-prinsip dasar hukum"},
    {"keratinization",        "Proses pembentukan keratin pada sel kulit sehingga menjadi keras dan pelindung"},
    {"biogeochemical",        "Berkaitan dengan siklus unsur kimia yang melibatkan komponen biologis dan geologis"},
    {"chemiluminescence",     "Emisi cahaya sebagai hasil dari reaksi kimia tanpa menggunakan panas"},
    {"deoxyribonucleic",      "Berkaitan dengan DNA, molekul yang membawa informasi genetik makhluk hidup"},
    {"electromagnetism",      "Gaya fundamental yang mencakup interaksi antara muatan listrik dan medan magnet"},
    {"photosynthetically",    "Dengan cara atau melalui proses fotosintesis untuk menghasilkan energi"},
    {"superconductivity",     "Fenomena hilangnya hambatan listrik sepenuhnya pada suhu sangat rendah"},
    {"thermodynamically",     "Berkaitan dengan prinsip-prinsip hukum termodinamika dalam sistem fisika"},
    {"unconstitutional",      "Bertentangan dengan atau melanggar konstitusi atau undang-undang dasar negara"},
    {"underrepresented",      "Kondisi suatu kelompok yang memiliki representasi lebih sedikit dari seharusnya"},
    {"biostratigraphically",  "Berkaitan dengan penentuan usia lapisan batuan berdasarkan fosil yang terkandung"}
};
