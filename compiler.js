class TexesCompiler {
    constructor() {
        this.examples = {
            documento: `documento_inicio();

titulo("Mi Primer Documento");
autor("Tu Nombre");
fecha("2024");

seccion("Introducción");
parrafo("Este es mi primer documento creado con el compilador LaTeX en español. Es fácil de usar y muy poderoso.");

subseccion("Características");
lista_simple();
elemento("Sintaxis en español natural");
elemento("Genera código LaTeX profesional");
elemento("Completamente gratuito");
fin_lista();

seccion("Uso");
parrafo("Simplemente escribe los comandos en español y haz clic en Compilar.");

documento_fin();`,

            matematicas: `documento_inicio();

titulo("Fórmulas Matemáticas");
autor("Departamento de Matemáticas");

seccion("Ecuaciones Importantes");
parrafo("A continuación se presentan algunas ecuaciones fundamentales:");

subseccion("Álgebra");
ecuacion("x = \\\\frac{-b \\\\pm \\\\sqrt{b^2 - 4ac}}{2a}");
parrafo("La fórmula cuadrática para resolver ecuaciones de segundo grado.");

subseccion("Cálculo");
ecuacion("\\\\int_a^b f(x) dx = F(b) - F(a)");
ecuacion("\\\\frac{d}{dx} e^x = e^x");

subseccion("Física");
ecuacion("E = mc^2");
ecuacion("F = G \\\\frac{m_1 m_2}{r^2}");

subseccion("Estadística");
ecuacion("P(A|B) = \\\\frac{P(B|A) P(A)}{P(B)}");

documento_fin();`,

            listas: `documento_inicio();

titulo("Ejemplos de Listas");
autor("Guía de Formatos");

seccion("Lista Simple");
lista_simple();
elemento("Primer elemento de la lista");
elemento("Segundo elemento con texto más largo para demostrar el formato");
elemento("Tercer elemento");
fin_lista();

seccion("Lista Numerada");
lista_numerada();
elemento("Primer paso del proceso");
elemento("Segundo paso importante");
elemento("Tercer y último paso");
fin_lista();

seccion("Listas Anidadas");
lista_simple();
elemento("Elemento principal 1");
lista_simple();
elemento("Subelemento 1.1");
elemento("Subelemento 1.2");
fin_lista();
elemento("Elemento principal 2");
lista_numerada();
elemento("Paso 2.1");
elemento("Paso 2.2");
fin_lista();
fin_lista();

documento_fin();`,

            formato: `documento_inicio();

titulo("Formatos de Texto");
autor("Ejemplos de Estilos");

seccion("Estilos Básicos");
parrafo("Este es un párrafo normal.");
negrita("Este texto está en negrita.");
cursiva("Este texto está en cursiva.");
subrayado("Este texto está subrayado.");
tachado("Este texto está tachado.");

seccion("Combinaciones");
parrafo("Texto normal con parte en negrita y parte en cursiva mezclados.");

seccion("Alineación");
centrar("Este texto está centrado.");
derecha("Este texto está alineado a la derecha.");
izquierda("Este texto está alineado a la izquierda.");
justificar("Este texto está justificado en el documento.");

documento_fin();`,

            academico: `documento_inicio();

titulo("Inteligencia Artificial: Estado del Arte y Perspectivas Futuras");
autor("Dr. Ana María Rodríguez");
fecha("Marzo 2024");
resumen("Este artículo revisa los avances recientes en inteligencia artificial, con especial énfasis en el aprendizaje profundo y sus aplicaciones en diversos campos.");

seccion("Introducción");
parrafo("La inteligencia artificial (IA) ha experimentado un crecimiento exponencial en la última década, transformando industrias y redefiniendo las capacidades computacionales.");

subseccion("Objetivos del Estudio");
lista_numerada();
elemento("Revisar el estado actual de las técnicas de IA");
elemento("Identificar tendencias emergentes en el campo");
elemento("Analizar el impacto social y ético de la IA");
elemento("Proponer direcciones futuras de investigación");
fin_lista();

seccion("Marco Teórico");
subseccion("Aprendizaje Profundo");
parrafo("El aprendizaje profundo representa un subcampo del machine learning que utiliza redes neuronales con múltiples capas.");

ecuacion("y = f(Wx + b)");

parrafo("Donde y representa la salida, x la entrada, W los pesos, b el sesgo y f la función de activación.");

subseccion("Arquitecturas Principales");
lista_simple();
elemento("Redes Neuronales Convolucionales (CNN)");
elemento("Redes Neuronales Recurrentes (RNN)");
elemento("Transformers");
elemento("Autoencoders Variacionales");
fin_lista();

seccion("Metodología");
parrafo("Se realizó una revisión sistemática de la literatura publicada entre 2018 y 2024 en las principales conferencias y revistas del campo.");

subseccion("Criterios de Inclusión");
lista_numerada();
elemento("Artículos publicados en revistas indexadas");
elemento("Investigaciones con validación experimental");
elemento("Estudios que presenten contribuciones novedosas");
fin_lista();

seccion("Resultados y Discusión");
parrafo("Los resultados indican un crecimiento del 300% en publicaciones relacionadas con transformers desde 2020.");

subseccion("Aplicaciones Identificadas");
lista_simple();
elemento("Procesamiento de lenguaje natural");
elemento("Visión por computadora");
elemento("Diagnóstico médico asistido");
elemento("Vehiculos autónomos");
fin_lista();

seccion("Conclusiones");
parrafo("La IA continúa su trayectoria de crecimiento, con aplicaciones cada vez más diversas y sofisticadas. Sin embargo, es crucial abordar los desafíos éticos y de seguridad asociados.");

subseccion("Trabajo Futuro");
parrafo("Futuras investigaciones deberían enfocarse en el desarrollo de IA explicable y en la reducción de sesgos algorítmicos.");

documento_fin();`
        };

        this.initializeEvents();
        this.updateStatus('✅ Compilador listo - Escribe tu código .texes');
    }

    initializeEvents() {
        // Botón compilar
        document.getElementById('compileBtn').addEventListener('click', () => this.compile());
        
        // Botón limpiar
        document.getElementById('clearBtn').addEventListener('click', () => this.clearEditor());
        
        // Botón copiar
        document.getElementById('copyBtn').addEventListener('click', () => this.copyResult());
        
        // Botón descargar
        document.getElementById('downloadBtn').addEventListener('click', () => this.downloadLatex());
        
        // Selector de ejemplos
        document.getElementById('loadExample').addEventListener('change', (e) => {
            if (e.target.value) {
                this.loadExample(e.target.value);
            }
        });

        // Atajos de teclado
        document.getElementById('inputCode').addEventListener('keydown', (e) => {
            if (e.ctrlKey && e.key === 'Enter') {
                e.preventDefault();
                this.compile();
            }
        });
    }

    loadExample(exampleName) {
        const input = document.getElementById('inputCode');
        input.value = this.examples[exampleName] || '';
        this.updateStatus(`📚 Ejemplo "${exampleName}" cargado`);
        this.compile(); // Compilar automáticamente
    }

    clearEditor() {
        document.getElementById('inputCode').value = '';
        document.getElementById('outputCode').innerHTML = '<pre><code class="language-latex">// El código LaTeX generado aparecerá aquí</code></pre>';
        this.updateStatus('🧹 Editor limpiado');
    }

    compile() {
        const input = document.getElementById('inputCode').value.trim();
        
        if (!input) {
            this.updateStatus('⚠️ Escribe algún código .texes primero', 'warning');
            return;
        }

        this.updateStatus('🔄 Compilando...', 'loading');
        this.updateProgress(30);

        try {
            // Validación inicial
            if (!input.includes('documento_inicio()')) {
                throw new Error('El documento debe comenzar con documento_inicio()');
            }

            if (!input.includes('documento_fin()')) {
                throw new Error('El documento debe terminar con documento_fin()');
            }

            setTimeout(() => {
                try {
                    const latexCode = this.processTexes(input);
                    this.displayResult(latexCode);
                    this.updateProgress(100);
                    this.updateStatus('✅ Compilación exitosa', 'success');
                } catch (error) {
                    this.updateStatus(`❌ Error de sintaxis: ${error.message}`, 'error');
                    this.updateProgress(0);
                }
            }, 500);

        } catch (error) {
            this.updateStatus(`❌ Error: ${error.message}`, 'error');
            this.updateProgress(0);
        }
    }

    processTexes(texesCode) {
        let lines = texesCode.split('\n');
        let latex = '';
        let inList = false;
        let listType = '';
        let metadata = {
            titulo: 'Documento Generado',
            autor: 'Compilador LaTeX Web',
            fecha: '\\today',
            resumen: ''
        };

        // Procesar cada línea
        for (let i = 0; i < lines.length; i++) {
            let line = lines[i].trim();
            let lineNumber = i + 1;
            
            // Saltar líneas vacías y comentarios
            if (!line || line.startsWith('//')) continue;

            // Validar que las líneas de comando terminen con ;
            if (this.isCommandLine(line) && !line.endsWith(';')) {
                throw new Error(`Error en línea ${lineNumber}: Falta punto y coma (;) al final del comando`);
            }

            // Validar paréntesis balanceados
            if ((line.includes('(') || line.includes(')')) && !this.hasBalancedParentheses(line)) {
                throw new Error(`Error en línea ${lineNumber}: Paréntesis no balanceados`);
            }

            // Procesar comandos
            if (line.includes('documento_inicio()')) {
                latex = this.generatePreamble(metadata);
            }
            else if (line.includes('documento_fin()')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
                latex += '\\end{document}';
            }
            else if (line.includes('titulo(')) {
                metadata.titulo = this.extractArgument(line);
                latex = this.generatePreamble(metadata); // Regenerar preámbulo
            }
            else if (line.includes('autor(')) {
                metadata.autor = this.extractArgument(line);
                latex = this.generatePreamble(metadata);
            }
            else if (line.includes('fecha(')) {
                metadata.fecha = this.extractArgument(line);
                latex = this.generatePreamble(metadata);
            }
            else if (line.includes('resumen(')) {
                metadata.resumen = this.extractArgument(line);
            }
            else if (line.includes('seccion(')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
                latex += `\\section{${this.extractArgument(line)}}\n\n`;
            }
            else if (line.includes('subseccion(')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
                latex += `\\subsection{${this.extractArgument(line)}}\n\n`;
            }
            else if (line.includes('subsubseccion(')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
                latex += `\\subsubsection{${this.extractArgument(line)}}\n\n`;
            }
            else if (line.includes('capitulo(')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
                latex += `\\chapter{${this.extractArgument(line)}}\n\n`;
            }
            else if (line.includes('parrafo(')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
                latex += `${this.extractArgument(line)}\n\n`;
            }
            else if (line.includes('negrita(')) {
                const text = this.extractArgument(line);
                latex += `\\textbf{${text}}\n\n`;
            }
            else if (line.includes('cursiva(')) {
                const text = this.extractArgument(line);
                latex += `\\textit{${text}}\n\n`;
            }
            else if (line.includes('subrayado(')) {
                const text = this.extractArgument(line);
                latex += `\\underline{${text}}\n\n`;
            }
            else if (line.includes('tachado(')) {
                const text = this.extractArgument(line);
                latex += `\\sout{${text}}\n\n`;
            }
            else if (line.includes('centrar(')) {
                const text = this.extractArgument(line);
                latex += `\\begin{center}\n${text}\n\\end{center}\n\n`;
            }
            else if (line.includes('derecha(')) {
                const text = this.extractArgument(line);
                latex += `\\begin{flushright}\n${text}\n\\end{flushright}\n\n`;
            }
            else if (line.includes('izquierda(')) {
                const text = this.extractArgument(line);
                latex += `\\begin{flushleft}\n${text}\n\\end{flushleft}\n\n`;
            }
            else if (line.includes('justificar(')) {
                const text = this.extractArgument(line);
                latex += text + '\n\n';
            }
            else if (line.includes('lista_simple()')) {
                if (inList) {
                    latex += this.endList(listType);
                }
                latex += '\\begin{itemize}\n';
                inList = true;
                listType = 'itemize';
            }
            else if (line.includes('lista_numerada()')) {
                if (inList) {
                    latex += this.endList(listType);
                }
                latex += '\\begin{enumerate}\n';
                inList = true;
                listType = 'enumerate';
            }
            else if (line.includes('elemento(')) {
                if (inList) {
                    latex += `  \\item ${this.extractArgument(line)}\n`;
                }
            }
            else if (line.includes('fin_lista()')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
            }
            else if (line.includes('ecuacion(')) {
                if (inList) {
                    latex += this.endList(listType);
                    inList = false;
                }
                const formula = this.extractArgument(line);
                latex += `\\begin{equation}\n${formula}\n\\end{equation}\n\n`;
            }
            else if (line.includes('formula(')) {
                const formula = this.extractArgument(line);
                latex += `$${formula}$\n\n`;
            }
        }

        // Cerrar lista si todavía está abierta
        if (inList) {
            latex += this.endList(listType);
        }

        return latex;
    }

    isCommandLine(line) {
        const commands = [
            'documento_inicio', 'documento_fin', 'titulo', 'autor', 'fecha', 'resumen',
            'seccion', 'subseccion', 'subsubseccion', 'capitulo', 'parrafo',
            'negrita', 'cursiva', 'subrayado', 'tachado', 'centrar', 'derecha', 
            'izquierda', 'justificar', 'lista_simple', 'lista_numerada', 'elemento',
            'fin_lista', 'ecuacion', 'formula', 'salto_linea', 'salto_pagina', 'nueva_pagina'
        ];

        return commands.some(cmd => line.trim().startsWith(cmd + '('));
    }

    hasBalancedParentheses(line) {
        let balance = 0;
        for (let char of line) {
            if (char === '(') balance++;
            if (char === ')') balance--;
            if (balance < 0) return false;
        }
        return balance === 0;
    }

    extractArgument(line) {
        const match = line.match(/\((.*)\)/);
        if (match) {
            // Remover comillas y procesar escapes
            return match[1].replace(/^"(.*)"$/, '$1').replace(/\\\\(\\\\\\)?/g, '\\');
        }
        return line.replace(/^.*?\((.*)$/, '$1').replace(/"/g, '');
    }

    generatePreamble(metadata) {
        let preamble = `\\documentclass{article}
\\usepackage[utf8]{inputenc}
\\usepackage[spanish]{babel}
\\usepackage{amsmath}
\\usepackage{amssymb}
\\usepackage{ulem}
\\usepackage{graphicx}
\\usepackage{hyperref}

\\title{${metadata.titulo}}
\\author{${metadata.autor}}
\\date{${metadata.fecha}}

\\begin{document}

\\maketitle
`;

        if (metadata.resumen) {
            preamble += `\\begin{abstract}
${metadata.resumen}
\\end{abstract}

`;
        }

        return preamble;
    }

    endList(type) {
        if (type === 'itemize') {
            return '\\end{itemize}\n\n';
        } else if (type === 'enumerate') {
            return '\\end{enumerate}\n\n';
        }
        return '';
    }

    displayResult(latexCode) {
        const outputElement = document.getElementById('outputCode');
        
        // Limpiar el contenido anterior
        outputElement.innerHTML = '';
        
        // Crear elemento pre con código resaltado
        const pre = document.createElement('pre');
        const code = document.createElement('code');
        code.className = 'language-latex';
        code.textContent = latexCode;
        pre.appendChild(code);
        
        outputElement.appendChild(pre);
        
        // Resaltar sintaxis
        if (window.hljs) {
            hljs.highlightElement(code);
        }
        
        console.log('Código LaTeX generado y mostrado:', latexCode);
    }

    async copyResult() {
        const result = document.querySelector('#outputCode code')?.textContent;
        if (!result || result.includes('// El código LaTeX generado aparecerá aquí')) {
            this.updateStatus('⚠️ No hay código para copiar', 'warning');
            return;
        }

        try {
            await navigator.clipboard.writeText(result);
            this.updateStatus('📋 Código LaTeX copiado al portapapeles', 'success');
        } catch (err) {
            this.updateStatus('❌ Error al copiar: ' + err.message, 'error');
        }
    }

    downloadLatex() {
        const latex = document.querySelector('#outputCode code')?.textContent;
        if (!latex || latex.includes('// El código LaTeX generado aparecerá aquí')) {
            this.updateStatus('⚠️ No hay código LaTeX para descargar', 'warning');
            return;
        }

        const blob = new Blob([latex], { type: 'text/plain; charset=utf-8' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = 'documento-generado.tex';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);
        
        this.updateStatus('💾 Archivo .tex descargado', 'success');
    }

    updateStatus(message, type = 'info') {
        const statusElement = document.getElementById('statusText');
        statusElement.textContent = message;
        statusElement.className = '';
        
        if (type === 'success') statusElement.classList.add('status-success');
        if (type === 'error') statusElement.classList.add('status-error');
        if (type === 'warning') statusElement.classList.add('status-warning');
        if (type === 'loading') statusElement.classList.add('status-loading');
    }

    updateProgress(percent) {
        document.getElementById('progressBar').style.width = percent + '%';
    }
}

// Inicializar el compilador cuando se cargue la página
document.addEventListener('DOMContentLoaded', () => {
    window.compiler = new TexesCompiler();
});