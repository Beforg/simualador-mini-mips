
# 🛠️ Fluxo de Trabalho | Simulador MiniMips em C

Este documento descreve as etapas padronizadas para contribuição no projeto, garantindo a integridade da branch principal e a organização do histórico de commits.

## 1. Clonagem do Repositório
Para iniciar, clone o repositório focando na branch de desenvolvimento (`dev`):

```bash
git clone -b dev --single-branch <url-do-repositorio>
cd <nome-do-repositorio>
```

## 2. Criação de uma Branch Temporária
Nunca trabalhe diretamente na `dev`. Crie uma nova branch para sua feature ou correção:

```bash
git checkout -b nome-da-sua-branch
```

> **Exemplo:** `git checkout -b feature/implementa-decodificador`

## 3. Realizar Alterações no Código
Desenvolva as modificações necessárias no projeto conforme a tarefa proposta. Certifique-se de seguir os padrões de codificação em C estabelecidos pela equipe.

## 4. Adicionar e Commitar Alterações
Após concluir as mudanças, prepare os arquivos:

```bash
git add .
```

Crie um commit utilizando o padrão de **Conventional Commits**:

```bash
git commit -m "feat: implementa decodificador de instruções"
```

## 5. Enviar a Branch para o Remoto
Suba suas alterações para o servidor do GitHub:

```bash
git push -u origin nome-da-sua-branch
```

## 6. Abrir Pull Request (PR) para a Branch `dev`
1. Acesse o repositório no GitHub.
2. Clique no botão **"Compare & pull request"** que aparecerá no topo.
3. **Configuração do Merge:**
   - **Base:** `dev`
   - **Compare:** `nome-da-sua-branch`
4. **Documentação:**
   - Adicione um título claro.
   - Descreva brevemente o que foi implementado ou corrigido.
5. Clique em **"Create pull request"**.
